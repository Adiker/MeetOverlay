#!/usr/bin/env python3
import json, os, pathlib, socket, struct, subprocess, sys, tempfile
with tempfile.TemporaryDirectory() as directory:
    env = dict(os.environ, XDG_RUNTIME_DIR=directory)
    server = socket.socket(socket.AF_UNIX)
    server.bind(str(pathlib.Path(directory) / 'meetoverlay.sock'))
    server.listen()
    server.settimeout(5)
    proc = subprocess.Popen([sys.argv[1]], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
    try:
        connection, _ = server.accept()
        connection.settimeout(5)
        message = b'{"state":"muted","count":1}'
        frame = struct.pack('=I', len(message)) + message
        proc.stdin.write(frame[:2]); proc.stdin.flush()
        proc.stdin.write(frame[2:]); proc.stdin.flush()
        result = b''
        while b'\n' not in result: result += connection.recv(1024)
        assert json.loads(result) == {"state":"muted","count":1}
        proc.stdin.close()
        assert proc.wait(timeout=5) == 0
        assert proc.stdout.read() == b''
    finally:
        if proc.poll() is None: proc.kill(); proc.wait()
        server.close()
