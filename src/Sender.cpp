//
// Created by efarhan on 9/15/20.
//
#include <iostream>
#include "Packet.h"
#include "Sender.h"
#include "Channel.h"

void Sender::ReceivePacket(const Packet &packet)
{
    //TODO check if received packet is corrupted
    bool isAck = packet.data[0];
    byte seqNmb = packet.sequenceNmb;
    if (!CheckCorrupt(packet))
    {
        isAck = false;
       
    }
    if(isAck)
    {
        if(seqNmb == 1)
        {
            CalculateFirstRTT(packet.rtt);
        }
        else
        {
            CalculateNewRTT(packet.rtt);
        }
        //TODO manage internal state when receiving ack
        packet.sequenceNmb + 1;
    }
    else
    {
        isAck = false;
        packet.sequenceNmb - 1;
        //TODO manage internal state when receiving nak
    }
}

void Sender::SendPacket(const Packet &packet)
{
    channel_.SendToReceiver(packet);
}

void Sender::Send(std::string_view msg)
{
    size_t pktNumber = msg.size()/Packet::packetSize;
    if(pktNumber*Packet::packetSize != msg.size())
    {
        pktNumber++;
    }
    for(size_t i = 0; i < pktNumber;i++)
    {
        Packet p;
        for(size_t j = 0; j < Packet::packetSize; j++)
        {
            const auto index = i * Packet::packetSize + j;
            if(index == msg.size())
                break;
            const char c = msg[index];
            if(c == 0)
                break;
            p.data[j] = c;
        }
        p.sequenceNmb = i+1;
        p = Client::GenerateChecksum(p);
        sentPackets_.push_back(p);
    }
    lastSendSeqNmb_ = 1;
}

void Sender::Update(float dt)
{
    if(IsMessageSent())
    {
        return;
    }
    timer_ += dt;
    if(timer_ >= rto_)
    {
        OnTimeout();
    }
}

byte Sender::GetLastSendSeqNmb() const
{
    return lastSendSeqNmb_;
}

void Sender::CalculateFirstRTT(float r)
{
    byte g;
    byte max = 0;
    srtt_ = r;
    rttvar_ = r / 2;
    rto_ = srtt_ + max* (g, k_ * rttvar_);
    
    //TODO Calculate SRTT, RTTVAR and RTO according to RFC 6298
}

void Sender::CalculateNewRTT(float r)
{
    rttvar_ = (1 - beta_) * rttvar_ + beta_ * abs( srtt_ - r);
    srtt_ = (1 - alpha_) * srtt_ + alpha_ * r;
    //TODO Calculate SRTT, RTTVAR and RTO according to RFC 6298
}

bool Sender::IsMessageSent() const {
    return lastSendSeqNmb_ == sentPackets_.size() + 1;
}

void Sender::OnTimeout()
{
    //int g;
    //int max = 0;
    Packet& packet = sentPackets_[lastSendSeqNmb_-1];
    packet.rtt = packetDelay_;
    SendPacket(packet);
    //TODO update RTT and timer
    Update(timer_);
    Update(GetSrtt());
    //rto_ = srtt_ + max*(g, k_ * rttvar_);
}

void Sender::SendNewPacket(float packetDelay)
{
    if(IsMessageSent())
        return;
    packetDelay_ = packetDelay;
    Packet& packet = sentPackets_[lastSendSeqNmb_-1];
    packet.rtt = packetDelay;
    SendPacket(packet);
}

float Sender::GetSrtt() const {
    return srtt_;
}

float Sender::GetRttvar() const {
    return rttvar_;
}

float Sender::GetRto() const {
    return rto_;
}

size_t Sender::GetPacketNmb() const {
    return sentPackets_.size();
}
