"""
Local HTTPS server for the Hackpad Key Tester.

Required for Web Serial API access in Chromium-based browsers.

Usage:
    python serve_https.py [--port 8443]

After starting, visit: https://localhost:8443/key-tester.html
(Your browser will warn about the self-signed cert -- click through to proceed.)
"""

import http.server
import ssl
import argparse
import os
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent

def main():
    parser = argparse.ArgumentParser(description="Serve key-tester.html over HTTPS")
    parser.add_argument("--port", type=int, default=8443, help="Port to listen on (default: 8443)")
    args = parser.parse_args()

    handler = http.server.SimpleHTTPRequestHandler

    server = http.server.HTTPServer(("0.0.0.0", args.port), handler)

    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ctx.load_cert_chain(certfile=SCRIPT_DIR / "cert.pem", keyfile=SCRIPT_DIR / "key.pem")
    server.socket = ctx.wrap_socket(server.socket, server_side=True)

    print(f"HTTPS server running at https://localhost:{args.port}/key-tester.html")
    print("   (Self-signed cert -- accept the browser warning to proceed)")
    print("   Press Ctrl+C to stop.\n")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down.")
        server.server_close()

if __name__ == "__main__":
    main()
