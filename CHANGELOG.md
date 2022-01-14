# 4.0.0
 - Refactor constants to not use 0xF0 ... 0xFF space, these are reserved for customization.

# 3.2.1
 - Fix endpoint 5 negative temperatures being compiler-dependent.

# 3.2.0
 - Add security-related endpoint 2A

# 3.1.1
 - Conditionally compile endpoints to save flash on constrained targets.

# 3.1.0
 - Add encrypted data formats 0x08, 0xFA
 - Fix Dataformat 5 overflow bugs.

# 3.0.0
 - Stable release

# 0.2.0
 - Add UART-specific `0xCA` endpoint for Ruuvi Gateway.

# 0.1.0
 - Add iBeacon and Ruuvi RAWv1, RAWv2 (0x03, 0x05).
