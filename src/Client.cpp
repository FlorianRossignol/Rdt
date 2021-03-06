
#include <cstddef>
#include "Client.h"

bool Client::CheckCorrupt(const Packet &packet)
{
    byte checksum = 0;
    /*******************************
     * Begin Generate the checksum
     */
    Packet generatedPacket = GenerateChecksum(packet);
    
    if (generatedPacket.checksum == packet.checksum)
    {
        return true;
    }
    else
    {
        return false;
    }
    /*******************************
     * End Generate the checksum
     */

    /*******************************
     * Compare the checksum in the packet and the generated one
     * and return the result
     */
    
}

Packet Client::GenerateChecksum(const Packet &packet)
{
    Packet generatedPacket = packet;
    byte checksum = 0;
    checksum = generatedPacket.sequenceNmb;
    for (auto data : generatedPacket.data)
    {
        checksum += data;
    }
    generatedPacket.checksum = checksum;
    /*******************************
     * Begin Generate the checksum
     */
    
    /*******************************
     * End Generate the checksum
     */
    return generatedPacket;
}

