#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int venue_counter = 1; // Initialise counter for venue and event, for ease of encoding
int event_counter = 1;

struct eventnode // Define struct for events (linked list node)
{
    struct tm starttime;
    struct tm endtime;
    int event_code;
    char description[500], name[50];
    struct eventnode *next;
    struct venuenode *parent;
};
typedef struct eventnode *ENODE;

struct venuenode // Define struct for venue (linked list node/ tree node)
{
    int venue_code;
    char venue[40];
    struct venuenode *next_venue; // Pointer to the venue that the event is held in.
    struct eventnode *first_event;
};
typedef struct venuenode *VNODE;

struct clash // Define struct for event clashes (linked list)
{
    ENODE event1;
    ENODE event2;
    struct clash *next;
};
typedef struct clash *CLASH;

struct free_time // Define struct for free time (linked list)
{
    ENODE event1;
    ENODE event2;
    float time_diff;
    struct free_time *next;
};
typedef struct free_time *TIME;

VNODE head_root = NULL;  // * root node of the main tree
CLASH head_clash = NULL; // * head pointer of the linked list of event clashes
TIME head_time = NULL;   // * head pointer of the linked list of free time

ENODE create_event() // Function  to create a new empty event node and return its address.
{
    ENODE new_event;
    new_event = (ENODE)malloc(sizeof(struct eventnode));
    // take input of all details of new event
    // DD-MM-YYYY HH:MM in start time
    scanf("%d-%d-%d %d:%d", &new_event->starttime.tm_mday, &new_event->starttime.tm_mon, &new_event->starttime.tm_year, &new_event->starttime.tm_hour, &new_event->starttime.tm_min);
    // HH:MM of end time
    scanf("%d:%d", &new_event->endtime.tm_hour, &new_event->endtime.tm_min);
    new_event->starttime.tm_year -= 1900;
    new_event->starttime.tm_mon -= 1;
    gets(new_event->description);
    gets(new_event->name);
    new_event->event_code = event_counter++; // increment  counter each time an event is created
    new_event->parent = NULL;
    new_event->next = NULL;
    return new_event; // * returns address of new event
}

VNODE create_venue() // Function to create a new empty venue node and return its address.
{
    VNODE new_venue;
    new_venue = (VNODE)malloc(sizeof(struct venuenode));
    new_venue->venue_code = venue_counter++; // increment venue counter each time a venue is created
    gets(new_venue->venue);
    new_venue->next_venue = NULL; // Standard definition of the tree
    new_venue->first_event = NULL;
    return new_venue;
}

void insert_venue(VNODE new_venue) // Function to add a new venue to the venueset
{
    if (head_root != NULL)
    {
        VNODE temp = head_root;
        while (temp->next_venue != NULL) //* Loop to iterate to the end of venueset.
        {
            temp = temp->next_venue;
        }
        temp->next_venue = new_venue;
        free(temp);
    }
    else
    {
        head_root = new_venue; // * If venueList == empty, add first event
    }
}

void insert_clash(CLASH new_clash) // Adds clashes between events to the event clash list.
{
    if (head_clash != NULL)
    {
        CLASH temp = head_clash;
        while (temp->next != NULL)
        {
            temp = temp->next; //* Loop to iterate to the end of clashset.
        }
        temp->next = new_clash;
        free(temp);
    }
    else
    {
        head_clash = new_clash; // * If clashList == empty, add first clash.
    }
}

void insert_time(TIME new_time) // Inserts free time into the freeTimeList between two events event.
{
    if (head_time != NULL)
    {
        CLASH temp = head_time;
        while (temp->next != NULL)
        {
            temp = temp->next; //* Loop to iterate to the end of timeset.
        }
        temp->next = new_time;
        free(temp);
    }
    else
    {
        head_time = new_time; // * If freeTimeList == empty, add first time slot.
    }
}

void deleteClash(ENODE curr) // To delete a time clash in case a certain event is deleted. (To prevent Dangling pointers)
{
    CLASH currentClash = NULL, prevClash = head_clash; // using two-pointer algo, to iterate the list.
    if (prevClash)                                     // * Checking for empty list.
    {
        // Check if the specified event is involved in the time clash.
        if (((prevClash->event1)->event_code == curr->event_code) || ((prevClash->event2)->event_code == curr->event_code))
        {
            // ^ free all pointers, but do not delete the event nodes themselves.
            prevClash->event1 = NULL;
            prevClash->event2 = NULL;
            free(prevClash->event1);
            free(prevClash->event2);
        }
        currentClash = prevClash->next;
    }
    if (currentClash) // * Checking for single node in list.
    {
        while (currentClash != NULL) // Iterate to the end.
        {                            // Check if the specified event involved in the time clash.
            if (((currentClash->event1)->event_code == curr->event_code) || ((currentClash->event2)->event_code == curr->event_code))
            { // ^ free all pointers, but do not delete the event nodes themselves.
                currentClash->event1 = NULL;
                currentClash->event2 = NULL;
                free(currentClash->event1);
                free(currentClash->event2);
                prevClash->next = currentClash->next; // mainatin continuity in the clashlist.
                free(currentClash);                   // deleting the clash node from the memory.
                currentClash = prevClash->next;       // intialise to the next node.
            }
            else
            {
                currentClash = currentClash->next; // Move to next node if condition false
                prevClash = prevClash->next;
            }
        }
    }
    prevClash = NULL;
    free(currentClash);
    free(prevClash);
}

void deleteTime(ENODE curr) // To delete/update a time slot in case a certain event is deleted. (To prevent Dangling pointers)
{
    TIME currentTime = NULL, prevTime = head_time; // using two-pointer algo, to iterate the list.
    if (prevTime)                                  // * Checking for empty list.
    {
        // Check to see if the specified event is in the first free-time node. (leading or trailing event)
        if (((prevTime->event1)->event_code == curr->event_code) || ((prevTime->event2)->event_code == curr->event_code))
        {
            // Check to see if there is another event after event2 in it's linkedlist.
            if ((prevTime->event2)->next)
            {
                // If yes then, change the free time duration.
                prevTime->event2 = (prevTime->event2)->next;
                prevTime->time_diff = difftime((prevTime->event1)->endtime.tm_hour, (prevTime->event2)->starttime.tm_hour);
                currentTime = prevTime->next;
            }
            else
            {
                // Else delete the time slot node.
                prevTime->event1 = NULL;
                prevTime->event2 = NULL;
                free(prevTime->event1);
                free(prevTime->event2);
            }
        }
    }
    if (currentTime) // * Checking for more than one node in LL.
    {
        while (currentTime != NULL) // Iterate to the end.
        {                           // Check to see if the specified event is in the first free-time node. (leading or trailing event)
            if (((currentTime->event1)->event_code == curr->event_code) || ((currentTime->event2)->event_code == curr->event_code))
            {
                // Check to see if there is another event after event2 in it's linkedlist.
                if ((currentTime->event2)->next)
                {
                    // If yes then, change the free time duration.
                    currentTime->event2 = (currentTime->event2)->next;
                    currentTime = currentTime->next;
                    currentTime->time_diff = difftime((currentTime->event1)->endtime.tm_hour, (currentTime->event2)->starttime.tm_hour);
                }
                else
                {
                    // Else delete the time slot node.
                    currentTime->event1 = NULL;
                    currentTime->event2 = NULL;
                    free(currentTime->event1);
                    free(currentTime->event2);
                }
            }
            else
            {
                // If the event is not in current node, move to next.
                currentTime = currentTime->next;
                prevTime = prevTime->next;
            }
        }
    }
    prevTime = NULL;
    free(currentTime);
    free(prevTime);
}

void deleteEvent(int eCode, int vNum) // To delete an event from the list.
{
    VNODE ven = head_root;
    ENODE ptr, curr;
    while (ven->venue_code != vNum) // Find required venue.
    {
        ven = ven->next_venue;
    }
    ptr = ven->first_event;       // * Use two-pointer algo.
    if (ptr->event_code == eCode) // checking for single node in list.
    {
        head_root->first_event = ptr->next;
        deleteClash(ptr); // Call functions to delete time-clash and free-time slot nodes.
        deleteTime(ptr);
        free(ptr);
        return;
    }
    curr = ptr->next;
    if (curr == NULL) // if single node and node with code not found.
    {
        free(curr);
        ptr = NULL;
        free(ptr);
        return;
    }
    while ((curr->event_code != eCode) && (curr != NULL)) // * Iterate to required node.
    {
        ptr = ptr->next;
        curr = curr->next;
    }
    ptr->next = curr->next;
    deleteClash(curr); // Call functions to delete time-clash and free-time slot nodes.
    deleteTime(curr);
    free(curr);
}

ENODE insert_event_rear(int vNum) // To insert an event to the end of a list, specfic to a venue.
{
    VNODE ven;
    ENODE temp, curr;
    ven = head_root;
    while (ven->venue_code != vNum) // * Find required venue.
    {
        ven = ven->next_venue;
    }
    temp = create_event();
    temp->parent = ven;
    if (ven->first_event == NULL) // If no events present at all in that venue, add first.
    {
        ven->first_event = temp;
    }
    else
    {
        while (curr->next != NULL) // iterate to the end.
        {
            curr = curr->next;
        }
        curr->next = temp; // * Add event.
    }
    return temp;
}

ENODE insert_event_pos(int vNum, ENODE exist) // To insert an event at the specific position of a list, specfic to a venue.
{
    VNODE ven = head_root;
    ENODE temp, ptr;
    while (ven->venue_code != vNum) // * Find required venue.
    {
        ven = ven->next_venue;
    }
    temp = create_event();
    temp->parent = ven;
    if (ven->first_event == NULL)
    {
        temp = insert_event_rear(vNum); // If no events present at all in that venue, add first.
        return temp;
    }
    ptr = ven->first_event;
    // Iterate to the required position by comparing timestamps, * list is sorted in ascending order. *
    while ((temp->starttime.tm_hour >= ptr->starttime.tm_hour) && (temp->starttime.tm_min >= ptr->starttime.tm_min) && ptr->next != NULL)
    {
        ptr = ptr->next;
    }
    ptr->next = temp; // * Add event.
    return temp;
}

void clash_and_free() // To find the clashing events and the free-time slots.
{
    VNODE temp = head_root;
    ENODE curr;
    CLASH new_clash;
    while (temp->next_venue != NULL) // Iterate venue by venue.
    {
        curr = temp->first_event;
        while (curr->next != NULL) // For each venue, iterate by event.
        {
            if (curr->endtime.tm_hour > (curr->next)->starttime.tm_hour) // Check if earlier event ends after the succeeding one starts.
            {
                new_clash->event1 = curr; // Define all attributes of clashnode.
                new_clash->event2 = curr->next;
                new_clash->next = NULL;
                if (head_clash == NULL)
                {
                    head_clash = new_clash;
                }
                else
                {
                    insert_clash(new_clash); // * Add a new clash.
                }
            }
            else if (curr->endtime.tm_hour == (curr->next)->starttime.tm_hour) // If the event starting hours are same, check minutes.
            {
                if (curr->endtime.tm_min > (curr->next)->starttime.tm_min) // Check if earlier event ends after the succeeding one starts. (minutes)
                {
                    new_clash->event1 = curr; // Define all attributes of classhnode.
                    new_clash->event2 = curr->next;
                    new_clash->next = NULL;
                    if (head_clash == NULL)
                    {
                        head_clash = new_clash;
                    }
                    else
                    {
                        insert_clash(new_clash); // * Add a new clash.
                    }
                }
            }
            else
            {
                // Check if the free timeslot is greater than 1 hr.
                if (difftime((curr->next)->starttime.tm_hour, curr->endtime.tm_hour) >= 3600.00)
                {
                    // Add a new time slot to the free-time list.
                    TIME new_time;
                    new_time->event1 = curr;
                    new_time->event2 = curr->next;
                    // Calc the time slot duration.
                    new_time->time_diff = difftime((curr->next)->starttime.tm_hour, curr->endtime.tm_hour);
                    new_time->next = NULL;
                    if (head_time == NULL)
                    {
                        head_time = new_time;
                    }
                    else
                    {
                        insert_time(new_time);
                    }
                }
            }
            curr = curr->next;
        }
        temp = temp->next_venue;
    }
}

void display_clash_and_free() // Function to traverse the time-clash and free-time linked list and display the nodes.
{
    // Basic LL traversal
    CLASH temp_clash = head_clash;
    TIME temp_time = head_time;
    while (temp_clash->next != NULL)
    {
        printf("Venue: %s\n", temp_clash->event1->parent->venue);
        printf("Name of event 1: %s\tStart time: %d\tEnd time:%d", (temp_clash->event1)->name, (temp_clash->event1)->starttime, (temp_clash->event1)->endtime);
        printf("Name of event 2: %s\tStart time: %d\tEnd time:%d", (temp_clash->event2)->name, (temp_clash->event2)->starttime, (temp_clash->event2)->endtime);
        temp_clash = temp_clash->next;
    }
    printf("Free times:\n");
    while (temp_time->next != NULL)
    {
        printf("Name of event 1: %s\tStart time: %d\tEnd time:%d", (temp_time->event2)->name, (temp_time->event2)->starttime, (temp_time->event1)->endtime);
        printf("Name of event 2: %s\tStart time: %d\tEnd time:%d", (temp_time->event2)->name, (temp_time->event2)->starttime, (temp_time->event2)->endtime);
        temp_time = temp_time->next;
    }
}

void reschedule(int event_code_change, int venue_code_change) // Function to auto-reschedule events based on user input.
{                                                             // Take input of event code and venue code.
    TIME time_temp = head_time;
    ENODE curr_event, ptr;
    VNODE venue_temp = head_root;
    while ((venue_temp->next_venue != NULL) && (venue_temp->venue_code == venue_code_change)) // Iterate till correct venue.
    {
        venue_temp = venue_temp->next_venue;
    }
    ptr = venue_temp->first_event;
    if (ptr->event_code == event_code_change) // Check if the event is in the first position of the linked list of venue.
    {                                         // Iterate through the free-time list to see if the event to be recheduled can fit in any time slot.
        while (time_temp != NULL)
        {
            if (time_temp->time_diff >= difftime(ptr->endtime.tm_hour, ptr->starttime.tm_hour))
            {
                break;
            }
            time_temp = time_temp->next;
        }
        if (time_temp)                     // Check if time slot is null. If not null then reschedule event to timeslot.
        {                                  // * Access the new location of the event with pointers from time-slot node.
            time_temp->event1->next = ptr; // * Edit the events linked list.
            ptr->next = time_temp->event2;
            venue_temp->first_event = ptr->next;
            ptr->parent = time_temp->event1->parent;
            deleteClash(ptr); // Delete the time clash and free-time nodes, becuse it has been resolved.
            deleteTime(ptr);
            return;
        }
        // ! else{
        // * show user that there are no timeslots that can accomodate the event
        // ! }
    }
    else
    {
        curr_event = ptr->next;
        while ((curr_event != NULL) && (curr_event->event_code == event_code_change)) // Iterste through all events
        {                                                                             // in this venue until we find the correct event.
            curr_event = curr_event->next;
            ptr = ptr->next;
        }
        if (curr_event) // Check if current event pointer is null. (i.e. Event is found or not)
        {               // Iterate through the frr-time list to see if the event to be rexcheduked can fit in any time slot.
            while ((time_temp != NULL) && (time_temp->time_diff >= difftime(ptr->endtime.tm_hour, ptr->starttime.tm_hour)))
            {
                time_temp = time_temp->next;
            }
            if (time_temp)                            // Check if time slot is null. If not null then reschedule event to timeslot.
            {                                         // * Access the new location of the event with pointers from time-slot node.
                time_temp->event1->next = curr_event; // * Edit the events linked list.
                curr_event->next = time_temp->event2;
                ptr->next = curr_event->next;
                curr_event->parent = time_temp->event1->parent;
                deleteClash(curr_event); // Delete the time clash and free-time nodes, becuse it has been resolved.
                deleteTime(curr_event);
                return;
            }
        }
    }
}

void main()
{
}