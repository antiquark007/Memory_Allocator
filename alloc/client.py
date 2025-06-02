import socket
import threading
import json
import time

class P2PClient:
    def __init__(self):  # Fixed: double underscores
        self.server_conn = None
        self.peers = {}
        self.my_port = 6000
        self.my_ip = socket.gethostbyname(socket.gethostname())
        self.broadcast_port = 5353
        self.server_port = 5050  # Must match server's port
        self.running = True
        self.setup_p2p_listener()  # Initialize this last after all attributes are set

    def setup_p2p_listener(self):
        """Start listening for incoming peer connections"""
        while self.my_port < 6010:  # Try ports 6000-6009
            try:
                self.p2p_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.p2p_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                self.p2p_socket.bind(('0.0.0.0', self.my_port))
                self.p2p_socket.listen(5)
                print(f"P2P listener started on port {self.my_port}")
                threading.Thread(target=self.accept_peer_connections, daemon=True).start()
                return
            except Exception as e:
                print(f"Port {self.my_port} unavailable: {e}")
                self.my_port += 1
        raise Exception("Could not bind to any port")

    def accept_peer_connections(self):
        """Accept incoming P2P connections"""
        while self.running:
            try:
                conn, addr = self.p2p_socket.accept()
                peer_id = f"{addr[0]}:{addr[1]}"
                self.peers[peer_id] = conn
                print(f"Connected to peer: {peer_id}")
                threading.Thread(target=self.handle_peer, args=(conn, peer_id)).start()
            except Exception as e:
                if self.running:
                    print(f"Peer connection error: {e}")

    def discover_server(self):
        """Discover server using UDP broadcast"""
        print("Searching for server...")
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.settimeout(2)
        
        try:
            # Send broadcast 3 times
            for _ in range(3):
                sock.sendto(b"DISCOVER_P2P_SERVER", ('255.255.255.255', self.broadcast_port))
                time.sleep(0.5)
            
            # Check for responses
            sock.settimeout(3)
            data, addr = sock.recvfrom(1024)
            response = json.loads(data.decode())
            print(f"Server found at {response['server_ip']}:{response['server_port']}")
            return response['server_ip']
        except socket.timeout:
            print("Server discovery timeout")
            return None
        except Exception as e:
            print(f"Discovery error: {e}")
            return None
        finally:
            sock.close()

    def connect_to_server(self):
        """Connect to central server"""
        server_ip = self.discover_server()
        if not server_ip:
            return False
            
        try:
            self.server_conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_conn.connect((server_ip, self.server_port))
            
            # Send our connection info
            self.server_conn.sendall(json.dumps({
                'ip': self.my_ip,
                'port': self.my_port
            }).encode())
            
            # Get initial peer list
            data = self.server_conn.recv(1024)
            peers = json.loads(data.decode())
            self.connect_to_peers(peers)
            
            threading.Thread(target=self.handle_server_messages, daemon=True).start()
            return True
        except Exception as e:
            print(f"Server connection failed: {e}")
            return False

    def connect_to_peers(self, peer_list):
        """Connect to peers from server's list"""
        for peer in peer_list:
            peer_id = f"{peer['ip']}:{peer['port']}"
            if peer_id not in self.peers and peer_id != f"{self.my_ip}:{self.my_port}":
                try:
                    peer_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    peer_socket.connect((peer['ip'], peer['port']))
                    self.peers[peer_id] = peer_socket
                    threading.Thread(target=self.handle_peer, args=(peer_socket, peer_id)).start()
                    print(f"Connected to peer: {peer_id}")
                except Exception as e:
                    print(f"Failed to connect to peer {peer_id}: {e}")

    def handle_peer(self, conn, peer_id):
        """Handle communication with a peer"""
        try:
            while self.running:
                data = conn.recv(1024)
                if not data:
                    break
                print(f"\n[Message from {peer_id}]: {data.decode()}")
        except Exception as e:
            print(f"Peer {peer_id} error: {e}")
        finally:
            conn.close()
            if peer_id in self.peers:
                self.peers.pop(peer_id)
            print(f"Disconnected from peer: {peer_id}")

    def handle_server_messages(self):
        """Handle messages from server"""
        try:
            while self.running:
                data = self.server_conn.recv(1024)
                if not data:
                    break
                
                message = json.loads(data.decode())
                if 'peers' in message:
                    self.connect_to_peers(message['peers'])
                else:
                    print(f"Server message: {message}")
        except Exception as e:
            print(f"Server connection error: {e}")
        finally:
            if self.server_conn:
                self.server_conn.close()

    def send_to_peer(self, peer_id, message):
        """Send message to specific peer"""
        if peer_id in self.peers:
            try:
                self.peers[peer_id].sendall(message.encode())
            except Exception as e:
                print(f"Failed to send to {peer_id}: {e}")
                if peer_id in self.peers:
                    self.peers.pop(peer_id)

    def broadcast(self, message):
        """Send message to all peers"""
        for peer_id, conn in list(self.peers.items()):
            try:
                conn.sendall(message.encode())
            except Exception as e:
                print(f"Failed to send to {peer_id}: {e}")
                if peer_id in self.peers:
                    self.peers.pop(peer_id)

    def shutdown(self):
        """Clean shutdown"""
        self.running = False
        
        # Close all peer connections
        for peer_id, conn in list(self.peers.items()):
            try:
                conn.close()
            except:
                pass
            self.peers.pop(peer_id, None)
        
        # Close server connection
        if self.server_conn:
            try:
                self.server_conn.close()
            except:
                pass
        
        # Close listener socket
        if hasattr(self, 'p2p_socket'):
            try:
                self.p2p_socket.close()
            except:
                pass

if __name__ == "__main__":
    client = P2PClient()
    try:
        if client.connect_to_server():
            print("\nCommands:")
            print("1. List peers")
            print("2. Send to peer")
            print("3. Broadcast")
            print("4. Exit")
            
            while True:
                choice = input("\nEnter choice: ")
                if choice == '1':
                    print("Connected peers:", list(client.peers.keys()))
                elif choice == '2':
                    peer_id = input("Enter peer ID: ")
                    message = input("Enter message: ")
                    client.send_to_peer(peer_id, message)
                elif choice == '3':
                    message = input("Enter broadcast message: ")
                    client.broadcast(message)
                elif choice == '4':
                    break
    except KeyboardInterrupt:
        print("\nShutting down...")
    finally:
        client.shutdown()