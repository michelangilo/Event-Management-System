// events.js

// Data structures
let venueCounter = 1;
let eventCounter = 1;

let venues = [];
let headClashes = null;
let headFreeTimes = null;

// Function to create an event
document.getElementById('addEventButton').addEventListener('click', createEvent);

function createEvent() {
    const newEvent = {
        starttime: document.getElementById('eventStartTime').value,
        endtime: document.getElementById('eventEndTime').value,
        eventCode: eventCounter++,
        description: document.getElementById('eventDescription').value,
        name: document.getElementById('eventName').value,
        parent: null,
        next: null
    };

    const venueCode = document.getElementById('venueCode').value;
    const venue = venues.find(v => v.venueCode === parseInt(venueCode));

    if (!venue) {
        showError('Venue not found.');
        return;
    }

    newEvent.parent = venue;

    if (!venue.firstEvent) {
        venue.firstEvent = newEvent;
    } else {
        let currentEvent = venue.firstEvent;
        while (currentEvent.next) {
            currentEvent = currentEvent.next;
        }
        currentEvent.next = newEvent;
    }

    showSuccess('Event added successfully.');
}

// Function to delete an event
function deleteEvent() {
    const deleteEventCode = parseInt(document.getElementById('deleteEventCode').value);
    const deleteEventVenueCode = parseInt(document.getElementById('deleteEventVenue').value);

    const venue = venues.find(v => v.venueCode === deleteEventVenueCode);

    if (!venue) {
        showError('Venue not found.');
        return;
    }

    let currentEvent = venue.firstEvent;
    let previousEvent = null;

    while (currentEvent && currentEvent.eventCode !== deleteEventCode) {
        previousEvent = currentEvent;
        currentEvent = currentEvent.next;
    }

    if (!currentEvent) {
        showError('Event not found.');
        return;
    }

    if (!previousEvent) {
        venue.firstEvent = currentEvent.next;
    } else {
        previousEvent.next = currentEvent.next;
    }

    // Delete clashes and free times related to the deleted event
    deleteClash(currentEvent);
    deleteTime(currentEvent);

    showSuccess('Event deleted successfully.');
}

// Function to add a venue
function addVenue() {
    const venueName = document.getElementById('venueName').value;

    const newVenue = {
        venueCode: venueCounter++,
        venue: venueName,
        nextVenue: null,
        firstEvent: null
    };

    insertVenue(newVenue);  // Call insertVenue to add the new venue to the list

    venues.push(newVenue);   // Add the new venue to the venues array

    showSuccess('Venue added successfully.');
}

// Function to insert a venue to the end of the venue list
function insertVenue(newVenue) {
    if (venues.length === 0) {
        venues.push(newVenue);
    } else {
        let temp = venues[0];
        while (temp.nextVenue !== null) {
            temp = temp.nextVenue;
        }
        temp.nextVenue = newVenue;
    }
}

function insertEventRear() {
    let venue = headRoot;
    while (venue.venueCode !== vNum) {
        venue = venue.nextVenue;
    }

    let newEvent = createEvent();
    newEvent.parent = venue;

    if (venue.firstEvent === null) {
        venue.firstEvent = newEvent;
    } else {
        let currentEvent = venue.firstEvent;
        while (currentEvent.next !== null) {
            currentEvent = currentEvent.next;
        }
        currentEvent.next = newEvent;
    }
}



// Function to find clashes and free time
function clashAndFree() {
    let temp = venues[0];
    let curr;

    while (temp.nextVenue !== null) {
        curr = temp.firstEvent;
        
        while (curr.next !== null) {
            if ((curr.endtime.slice(0, 2)) > curr.next.starttime.slice(0, 2)) {
                let newClash = {
                    event1: curr,
                    event2: curr.next,
                    next: null
                };

                if (headClashes === null) {
                    headClashes = newClash;
                } else {
                    insertClash(newClash);
                }
            } else {

                if (((curr.next.starttime.slice(0, 2)) - (curr.endtime.slice(0, 2)))>= 1 ){
                    let newTime = {
                        event1: curr,
                        event2: curr.next,
                        timeDiff: (curr.next.starttime - curr.endtime) / 1000,
                        next: null
                    };

                    if (headFreeTimes === null) {
                        headFreeTimes = newTime;
                    } else {
                        insertFreeTime(newTime);
                    }
                }
            }
            curr = curr.next;
        }
        temp = temp.nextVenue;
    }
}

// Function to delete clashes related to the deleted event
function deleteClash(curr) {
    headClashes = deleteNodeFromList(headClashes, curr);
}

// Function to delete free times related to the deleted event
function deleteTime(curr) {
    headFreeTimes = deleteNodeFromList(headFreeTimes, curr);
}

// Function to display clashes and free time
function displayClashAndFree() {
    clashAndFree();
    let tempClash = headClashes;
    let tempTime = headFreeTimes;

    if (tempClash !== null) {
        showSuccess("Clashes:");
        while (tempClash !== null) {
            showSuccess("Venue: " + tempClash.event1.parent.venue);
            showSuccess("Name of event 1: " + tempClash.event1.name + "\tStart time: " + tempClash.event1.starttime + "\tEnd time: " + tempClash.event1.endtime);
            showSuccess("Name of event 2: " + tempClash.event2.name + "\tStart time: " + tempClash.event2.starttime + "\tEnd time: " + tempClash.event2.endtime);
            tempClash = tempClash.next;
        }
    } else {
        showSuccess("No clashes found.");
    }

    if (tempTime !== null) {
        showSuccess("Free time Found:");
        while (tempTime !== null) {
            showSuccess("Name of event 1: " + tempTime.event1.name + "\tStart time: " + tempTime.event1.starttime + "\tEnd time: " + tempTime.event1.endtime);
            showSuccess("Name of event 2: " + tempTime.event2.name + "\tStart time: " + tempTime.event2.starttime + "\tEnd time: " + tempTime.event2.endtime);
            showSuccess("Total free time Found: "+ ((tempTime.event2.starttime.slice(0, 2))-(tempTime.event1.endtime.slice(0, 2)))+" Hours");
            tempTime = tempTime.next;
        }
    } else {
        showSuccess("No free times found.");
    }
}

// Function to delete a node from the linked list
function deleteNodeFromList(head, nodeToDelete) {
    if (!head) {
        return null;
    }

    if (head === nodeToDelete) {
        return head.next;
    }

    let current = head;
    let previous = null;

    while (current && current !== nodeToDelete) {
        previous = current;
        current = current.next;
    }

    if (!current) {
        return head;
    }

    previous.next = current.next;
    return head;
}

// Function to insert a new clash node in the linked list
function insertClash(newClash) {
    let current = headClashes;

    while (current.next !== null) {
        current = current.next;
    }

    current.next = newClash;
    newClash.next = null; 
}

// Function to insert a new free time node in the linked list
function insertFreeTime(newTime) {
    let current = headFreeTimes;

    while (current.next !== null) {
        current = current.next;
    }

    current.next = newTime;
    newTime.next = null; 
}

// Function to reschedule an event
function rescheduleEvent() {
    const rescheduleEventCode = parseInt(document.getElementById('rescheduleEventCode').value);
    const rescheduleVenueCode = parseInt(document.getElementById('rescheduleVenueCode').value);

    const venue = venues.find(v => v.venueCode === rescheduleVenueCode);

    if (!venue) {
        showError('Venue not found.');
        return;
    }

    let currEvent = venue.firstEvent;
    let ptr = null;

    while (currEvent) {
        if (currEvent.eventCode === rescheduleEventCode) {
            ptr = currEvent;
            break;
        }
        currEvent = currEvent.next;
    }

    if (!ptr) {
        showError('Event not found.');
        return;
    }

    let timeTemp = headFreeTimes;
    let rescheduled = false;
    
    while (timeTemp) {
        // Calculate the time difference in hours
        const timeDiffHours = timeTemp.timeDiff / 3600;

        if (timeDiffHours >= (parseInt(ptr.endtime.slice(0, 2)) - parseInt(ptr.starttime.slice(0, 2)))) {
            timeTemp.event1.next = ptr;
            ptr.next = timeTemp.event2;
            venue.firstEvent = ptr.next;
            ptr.parent = timeTemp.event1.parent;

            deleteClash(ptr);
            deleteTime(ptr);

            showSuccess('Event rescheduled successfully.');
            rescheduled = true;
            break;
        }

        timeTemp = timeTemp.next;
    }

    
        showError('No suitable time slots found for rescheduling the event.');
    }




// Helper functions for displaying success and error messages
function showSuccess(message) {
    alert('Success: ' + message);
}

function showError(message) {
    alert('Error: ' + message);
}
