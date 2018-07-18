
/**

João Araújo, 2017

Function to check if there were any lost packets between the current and
previous packet. Uses the packet counter byte to achieve this result

**/

void check_packets(int current_packet,int *previous_packet,int *lost_packets){

  //Detect if any packet was lost and update total lost packets
  if(*previous_packet != -1) {	// if it is the first packet do not use the functions


    if(current_packet != 0){
      //if it is not the 0 index check if the current is the previous + 1
      if(current_packet != *previous_packet + 1){

        *lost_packets += 1;

        printf("Packets were lost. Packets lost counter: %d\n", *lost_packets);
      }

    } else {

      // packet is zero index so previous packet had to be 127
      if(*previous_packet != 127){

        *lost_packets += 1;

        printf("Packets were lost. Packets lost counter: %d\n", *lost_packets);

      }
    }
  }

  *previous_packet = current_packet;
}
