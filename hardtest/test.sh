#!/bin/bash

# Find the IP range of the current network
IP_RANGE=$(ip route | grep '^default' | awk '{print $3}')

# Scan the network for active devices
nmap -sP $IP_RANGE | grep '^Nmap scan report for'
