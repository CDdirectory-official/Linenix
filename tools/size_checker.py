#!/usr/bin/env python3
"""
File Path: tools/size_checker.py
Description: Automated Binary Integrity & Boot Sector Size Analyzer for TzeOS.
"""

import os
import sys

TARGET_IMAGE = "../os_image.bin"

def analyze_kernel_image():
    print("[PYTHON] Starting binary integrity scan...")
    
    if not os.path.exists(TARGET_IMAGE):
        print(f"[ERROR] Target image '{TARGET_IMAGE}' not found. Compile first, bozo!")
        sys.exit(1)
        
    file_size = os.path.getsize(TARGET_IMAGE)
    print(f"[INFO] Total Compiled Kernel Image Size: {file_size} bytes")
    
    # Check Master Boot Record (MBR) Sector constraints
    with open(TARGET_IMAGE, "rb") as f:
        boot_sector = f.read(512)
        
        if len(boot_sector) < 512:
            print("[ERROR] Boot sector is truncated or corrupted!")
            sys.exit(1)
            
        # Verify Magic Boot Signature (0x55 0xAA in little endian)
        magic_sig = boot_sector[510:512]
        if magic_sig == b"\x55\xAA":
            print("[SUCCESS] Boot Signature Detected: 0xAA55 (Sector 0 is bootable!)")
        else:
            print(f"[CRITICAL] Invalid Boot Signature: {magic_sig.hex()}. CPU will ignore this image!")
            sys.exit(1)

    # Output sizing alert boundaries
    print(f"[INFO] Kernel Payload Size: {file_size - 512} bytes dynamic space.")
    print("[STATUS] Integrity validation passed smoothly.")

if __name__ == "__main__":
    analyze_kernel_image()
