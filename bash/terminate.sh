#!/bin/bash

# Cross-platform script to close port 8080

# Function to print messages in color
print_message() {
    local color=$1
    local message=$2
    echo -e "\033[${color}m${message}\033[0m"
}

# Function to check if running with admin privileges
check_admin() {
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        # Windows (Git Bash or similar)
        net session > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            return 0
        else
            return 1
        fi
    else
        # Unix-like systems (Linux, macOS)
        if [ "$(id -u)" -eq 0 ]; then
            return 0
        else
            return 1
        fi
    fi
}

# Function to close port 8080
close_port() {
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        # Windows
        netstat -ano | findstr :8080 | awk '{print $5}' | while read PID; do
            if [ ! -z "$PID" ]; then
                print_message "36" "Attempting to terminate process with PID: $PID"
                taskkill //PID $PID //F
            fi
        done
    else
        # Unix-like systems
        lsof -ti:8080 | xargs -r kill -9
    fi
}

# Check for admin privileges
if ! check_admin; then
    print_message "31" "Please run this script with administrator privileges."
    exit 1
fi

print_message "32" "Script is running with administrator privileges."

# Attempt to close port 8080
print_message "36" "Attempting to close port 8080..."
close_port

# Verify if port is closed
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    # Windows
    if netstat -ano | findstr :8080 > /dev/null; then
        print_message "31" "Port 8080 is still in use. Manual intervention may be required."
    else
        print_message "32" "Port 8080 has been successfully closed."
    fi
else
    # Unix-like systems
    if lsof -i:8080 > /dev/null 2>&1; then
        print_message "31" "Port 8080 is still in use. Manual intervention may be required."
    else
        print_message "32" "Port 8080 has been successfully closed."
    fi
fi
