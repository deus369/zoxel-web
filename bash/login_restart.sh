#!/bin/bash

# SSH into the remote server and execute commands
ssh root@194.195.251.84 << 'EOF'
  echo "restarting..."
  sleep 3
  systemctl restart zoxel-web.service
  sleep 3
EOF
