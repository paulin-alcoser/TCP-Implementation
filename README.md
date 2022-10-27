# Project 1: Transmission Control Protocol


### 1. Project Objectives

The task of this project is to implement TCP from the ground up. This project consists of two different tasks, mainly: reliable data transfer and congestion control. Both tasks have their own due date and are dependent on each other. This project is a group project and you must find exactly one partner to work with. Once you have settled on a partner, you need to inform us by sending an email to the TA and the professor, see the due date under submission details and policy. There will be a 10% penalty if the deadline of the group formation is crossed. Please read the complete project description carefully before you start so that you know exactly what is being provided and what functionality you are expected to add.

### 2. Task 1: Simplified TCP Sender/Receiver

The first task is to implement a "Reliable Data Transfer" protocol, following the description of section 3.5.4 from the textbook. The idea here is to build a simplified TCP sender and receiver that is capable of handling packet losses and retransmissions. The following functionalities must be implemented:

* Sending packets to the network based on a fixed sending window size (e.g. WND of 10 packets)
* Sending acknowledgments back from the receiver and handling what to do when receiving ACKs at the sender
* A timeout mechanism to deal with packet loss and retransmission 

For this task, there is no need to buffer out-of-order packets at the receiver and they can be simply discarded, i.e. no ACKs are sent for out-of-order packets. The out-of-order buffering will be implemented in task 2. However, lost packets must be retransmitted by the sender. For the timeout mechanism, you can assume a fixed timeout value that is appropriate for the emulated network scenario using MahiMahi (see below).

We have provided you with a simple (stop-and-wait) starter-code that consists of the following:
* rdt receiver: this holds the implementation of a simple reliable data transfer protocol (rdt) receiver
* rdt sender: this holds the implementation of a simple reliable data transfer protocol (rdt) sender
* Channel traces for emulating different network conditions 
 
The simple rdt protocol is implemented on top of the UDP transport protocol. During the lab session, Khalid showed you how to use the network emulator MahiMahi to test your sender and receiver functionality in an emulated network environment.

### 3. Task 2: TCP Congestion Control

The second task is to implement a congestion control protocol (similar to TCP Tahoe) for the sender and receiver of task 1. The implementation of the congestion control will consists of the following features:

* Slow-start
* Congestion avoidance
* Fast retransmit (no fast recovery)

The next subsections detail the requirements of the assignment. This high-level outline roughly mirrors the order in which you should implement functionality. For further details, please refer to the lecture slides or the textbook.

#### 3.1 Reliability and Sliding Window

The sender and the receiver have to maintain a sliding window, as shown in Figure 1.

![Figure 1](https://lh3.googleusercontent.com/liURdK1po8u_EhHkh0AQm07rB-gNFSb3IvorF8QT566NITcuAzyDzS1TLtTG2XLwDbdxcg=s170)

Figure 1: Sliding Window

The sender slides the window forward when it receives an ACK for a packet with the lowest sequence number in the sliding window. There is a sequence number associated with each packet and the following constraints are valid for the sender:

1. LastPacketAcked <= LastPacketSent
2. LastPacketSent <= LastPacketAvailable
3. LastPacketSent - LastPacketAcked <= WindowSize
4. Packets between LastPacketAcked and LastPacketAvailable must be “buffered”. You can either implement this by buffering the packets or by being able to regenerate them from the data file.

When the sender sends a data packet it starts a timer, if not already running. The timeout timer should be set based on the RTT estimator discussed in the lecture. It then waits for a certain amount of time to get the acknowledgement for the packet. Whenever the receiver receives a packet, it sends an ACK for NextPacketExpected. Upon receiving an out-of-order packet, the receiver must buffer the packet and send a duplicate ACK.

For example, upon receiving a packet with sequence number = 100, the reply would be “ACK 101”, but only if all packets with sequence numbers less than 100 have already been received. These ACKs are called cumulative ACKs. The sender has two ways to know if the packets it sent did not reach the receiver: either a time-out occurred, or the sender received “duplicate ACKs”.

If the sender sent a packet and did not receive an ACK for it before the timer expired, it retransmits the packet. If the sender sent a packet and received duplicate ACKs, it knows that the next expected packet (at least) was lost. To avoid confusion from reordering, a sender counts a packet lost only after 3 duplicate ACKs in a row.

#### 3.2 Congestion Control

Broadly speaking, the idea of TCP's congestion control is to determine how much capacity is available in the network, so it knows how many packets it can safely have “in-flight” at the same time. Once the sender has this many packets in transit, it uses the arrival of an ACK as a signal that one of its packets has left the network, and it is therefore safe to insert a new packet into the network without adding to the level of congestion. 

By using ACKs to pace the transmission of packets, TCP is said to be “self-clocking”. TCP's Congestion Control mechanism consists of the algorithms of Slow Start, Congestion Avoidance, Fast Retransmit and Fast Recovery. You can read more about these mechanisms in our textbook Section 3.7. In the first part of the project, your window size was fixed to 10 packets. The task of this second part is to dynamically determine the ideal congestion window size (CWND).

**Slow-start:** When a new connection is established with a host on another network, the CWND is initialized to one packet. Each time an ACK is received, CWND is increased by one packet. The sender keeps increasing CWND until the first packet loss is detected or until CWND reaches the value ssthresh (Slow-start threshold), after which it enters Congestion Avoidance mode (see below). For a new connection, the ssthresh is set to a very big value, we will use 64 packets. If a packet is lost in the slow-start phase, the sender sets ssthresh to max(CWND/2, 2) in case the client later returns to Slow-start again.

**Congestion Avoidance** slowly increases CWND until a packet loss occurs. The increase of CWND should be at most one packet per round-trip time (regardless how many ACKs are received in that RTT). That is, when the sender receives an ACK, it usually increases CWND by a fraction equal to 1/CWND. You may notice here that you need to use a float variable for the CWND, however when you send data you are always going to take the floor of the CWND. As soon as the entire window is acknowledged, only then these fractions would sum to a 1.0 and as a result the CWND would then have increased by 1. This is in contrast to Slow-start where CWND is incremented for each ACK. Recall that when the sender receives 3 duplicate ACK packets, you should assume that the packet with sequence number == acknowledgment number was lost, even if a timeout has not occurred yet. This process is called Fast Retransmit.

**Fast Retransmit:** After a Fast Retransmit, the ssthresh is set to max(CWND/2,2). CWND is then set to 1 and the Slow-start process starts again. There is NO need to implement the fast recovery mechanism.

#### 3.3 Graphing CWND 

Your sender implementation must generate a simple output file (named CWND.csv) that shows how the CWND varies over time. This will help you to debug and test your code, and it will also help us to grade your submission. The output format is simple, it should record the time when the window changed and the current CWND. You should also amend the plotting script (plot.py) to plot the CWND and how it evolves over time.

