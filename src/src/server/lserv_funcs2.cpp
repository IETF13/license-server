#define RECLAIM_INTERVAL 60 /*reclaim every 60 seconds*/
/*
*ticket_reclaim
*go through all tickets and reclaim ones belonging to dead process
*Results:none

*/
void ticket_reclaim()
{
	int i;
	char tick[BUFSIZ];
	for (i = 0; i < MAXUSERS; i++)
	{
		if ((ticket_arry[i] != TICKET_AVAIL) && (kill(ticket_array[i], 0) == -1) && (errno == ESRCH))
		{
			/*Process is gone-free up slot*/
			sprintf(tick, "%d.%d", ticket_array[i], i);
			narrate("freeing", tick, NULL);
			ticket_array[i] = TICKET_AVAIL;
			num_tickets_out--;
		}
	}
	alarm(RECLAIM_INTERVAL);/*reset alarm clock*/
}