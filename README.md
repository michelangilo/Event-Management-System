<h1>About the Project: eventSync</h1>
EventSync is a website made for creating and adding events for management in College Fests and other similar scenarios.<br>
<h2>How to Navigate</h2>
Open <b>index.html</b> for the Login/Sign-up page.<br>
Use the following credentials:
<ul>
  <li>Username: <i>user</i><br>Password: <i>user123</i></li>
  <li>Username: <i>admin</i><br>Password: <i>admin123</i></li>
</ul>
This will lead you to <b>user.html</b> or <b>admin.html</b> respectively.<br>
You can click on <b>Edit Events</b> on the nav bar to add, edit or delete events and see the events.
<h2>Backend</h2>
<ul>
  <li><b>event.js</b> contains the backend of the code.</li>
  <li>The backend was created in C language in <b>backend.c</b> which was later converted to JavaScript for ease of connectivity.</li>
  <li><b>backend.c</b> can be viewed for a better understanding of the concept of working of the backend.</li>
  <li>The backend works by using Tree Data Structure and adding events as nodes under the venues in ascending order of time and thus comparing and storing clashes and free slots based on the start and end time of events.</li>
  <li>These clashes and free times are stored in separate linked lists and are readjusted when needed.</li>
</ul>
