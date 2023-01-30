# Libraries

## broadcast-queue

This is a high performance single writer multi-reader queue, which is used for brodcast a message from writer to
readers. It employes sequnce lock concept to gain performance. seqlock (short for sequence lock) is a reader–writer consistent mechanism which avoids the problem of writer starvation. A seqlock consists of storage for saving a sequence number in addition to a lock.

## trariti

This is a high performance inter-thread queue that support Single/Multi producers and Single/Multi consumers mode.
