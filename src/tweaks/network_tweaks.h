#pragma once
#include "tweak_base.h"

// ─── Network Category ─────────────────────────────────────────────────────────

// Tweak 9: Disable Nagle's Algorithm (TcpAckFrequency + TCPNoDelay)
class DisableNagleTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Nagle's Algorithm"; }
    const char* Description() const override { return "Sets TCPNoDelay + TcpAckFrequency=1 for lower TCP latency."; }
    const char* Detail()      const override {
        return "Nagle's algorithm batches small TCP packets to improve throughput\n"
               "but adds up to 200 ms of latency on interactive connections.\n"
               "Disabling it helps online gaming and low-latency TCP streams.";
    }
    const char* Category()    const override { return "Network"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 10: Disable Network Throttling Index
class DisableNetworkThrottlingTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Network Throttling"; }
    const char* Description() const override { return "Removes the multimedia network throttle (NetworkThrottlingIndex=FFFFFFFF)."; }
    const char* Detail()      const override {
        return "Windows throttles non-multimedia network traffic when multimedia\n"
               "streams are active. Setting NetworkThrottlingIndex to 0xFFFFFFFF\n"
               "disables this throttling, improving latency for games.";
    }
    const char* Category()    const override { return "Network"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 11: Disable TCP Auto-Tuning
class DisableTCPAutoTuningTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable TCP Auto-Tuning"; }
    const char* Description() const override { return "Disables the TCP receive-window auto-tuning algorithm."; }
    const char* Detail()      const override {
        return "TCP auto-tuning dynamically adjusts the receive window size.\n"
               "On some systems or routers this causes erratic latency spikes.\n"
               "Disabling it sets a fixed conservative window that is more\n"
               "predictable for gaming traffic.";
    }
    const char* Category()    const override { return "Network"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable ECN Capability
class DisableECNTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable ECN Capability"; }
    const char* Description() const override { return "Disables Explicit Congestion Notification to reduce TCP overhead."; }
    const char* Detail()      const override {
        return "ECN allows routers to signal congestion without dropping packets.\n"
               "Some older routers and firewalls mishandle ECN-marked packets,\n"
               "causing retransmissions and latency spikes. Disabling it avoids\n"
               "these compatibility issues on problematic networks.";
    }
    const char* Category()    const override { return "Network"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable TCP Timestamps
class DisableTCPTimestampsTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable TCP Timestamps"; }
    const char* Description() const override { return "Removes TCP timestamp option to reduce per-packet overhead."; }
    const char* Detail()      const override {
        return "TCP timestamps (RFC 1323) add 12 bytes to each TCP header for\n"
               "RTT measurement and protection against wrapped sequence numbers.\n"
               "On fast local networks this overhead is unnecessary. Disabling\n"
               "reduces per-packet size and processing latency slightly.";
    }
    const char* Category()    const override { return "Network"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
