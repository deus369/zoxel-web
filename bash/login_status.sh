#!/bin/bash

# SSH into the remote server and execute commands
ssh root@194.195.251.84 << 'EOF'
  echo "status checking..."
  sleep 3
  systemctl status zoxel-web.service
  sleep 30
EOF
