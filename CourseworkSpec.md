# Coursework Spec

Due: 16th December, 12:00 noon

This coursework gives you an opportunity to implement the techniques and
principles you've learned over the course of Advanced Game Technologies, and
integrate them into a graphics framework, for a reasonably feature-complete
technical demonstration.

You will extend the graphical environment you created for the CSC8502 coursework
to include physical representations of the alien planetary environment and
objects within it. You will add mechanics to the environment to turn it into a
rudimentary game - the focus, as always, being technical achievement rather than
design.

You will not lose marks for using code taken from the course tutorials, though
further marks are available for extending that code and implementing/integrating
more advanced features than have been covered in the taught component of the
module. You should not take code wholesale from external sources (see Newcastle
University's rules regarding plagiarism), but you are free to reference external
publications and implement their principles in your own fashion.

The coursework is divided into two parts. The first part involves the
implementation of basic physics rules and gameplay mechanics. The second part is
to extend this into a more advanced physical representation, and integrate more
advanced technologies to further enhance the demo into a rudimentary game.

Your software MUST run at or above 30 FPS.

You MUST comment your code, identifying where the features required by both
coursework specifications have been implemented.

You MUST attend the coursework demonstration session (12 noon - 4 PM on Friday
16th December). If you cannot attend, you should let me know as soon as possible
to arrange an earlier demonstration.

NOTE: The specification of this coursework is necessarily complex, and care
should be taken when reading. The features listed in Part A should all be
implemented to achieve full marks for part A. A mark of at least 50% will be
awarded for Part B if the compulsory elements of Part B are implemented; a
higher mark will require the implementation of optional elements.

## Coursework Part A: Standard Features

- [x] You must implement a Newtonian solver capable of updating the linear and
      angular motion of 3-dimensional objects.
- [x] You must implement gravity, acting in the direction of the planet's
      centre of mass.
- [x] You must implement a 'Rest' state under which objects are not subject to
      physics updates (including gravity). This Rest state should end (the
      object should 'wake up') when an object suffers a collision.
- [x] You must add the capability to launch a textured, spherical projectile
      into the environment from the camera's PoV with the 'J' key (the camera
      must be able to move and turn, as in the CSC8502 coursework). This
      projectile will be subject to Newtonian physics.
- [x] You must implement basic collision detection and response algorithms
      which explicitly handle the following cases: sphere/sphere, sphere/plane,
      sphere/axis-aligned bounding box.
- [ ] You must add simple test scenarios into the environment which permit the
      testing of your basic collision detection and response algorithms. These
      will take the form of:
  - [ ] A sphere and a cube which begin the scenario in space, and at Rest.
        They should react to the impact of the fired projectile, waking up and
        responding to the collision as normal.
  - [ ] A horizontal quad and a vertical quad, represented as bounded planes. A
        sphere bouncing off either side of these quads should react
        accordingly, from both directions (as though firing the sphere at a
        wall). Note that as this will form a demo you show industry guests,
        you're encouraged to integrate the quads into your environment in a
        graphically pleasing form (for example, as elements of decoration on
        the planet, assuming your scaling permits).
- [ ] You must physically represent the Planet (see CSC8502 coursework) in some
      fashion (e.g., a sphere). The planet must rotate about its polar axis.
- [ ] You must add a target to the surface of the planet. This region will
      rotate as the planet does, and should be clearly indicated graphically
      (e.g., with a target reticule, or dart-board).
- [ ] You must implement a score counter. The score should increment by 100
      every time a launched sphere hits the target region of the planet.
- [ ] You should demonstrate awareness that the physical representation of an
      object does not need to perfectly match its graphical representation. An
      example of this principle in practise should be testable within the
      simulation (e.g., a bunny rabbit mesh represented, physically, as a few
      spheres)
- [x] At any point, a player should be able to end their session by pressing
      the 'X' key. The program should exit cleanly.

## Coursework Part B: Advanced Features

### Compulsory

- [x] You will enhance your physics simulation to include a more sophisticated
      collision detection and response approach, such as separating-axis
      theorem and manifolds. More marks will be awarded for an even more
      advanced approach (e.g.  GJK/EPA).
- [ ] Introduce additional objects of a more complex collision shape to the
      environment to demonstrate this functionality, which begin hovering at
      rest, and awaken when collided with.
- [ ] You will implement networking code to support a client-controlled entity
      within your environment. The client will execute on the same computer as
      your simulation (which will be considered the server), and will
      periodically adjust the acceleration of the client-controlled entity
      within the simulation. The entity can be as simple as a sphere.
      You shall include a demonstration of two-way communication between server
      and client (e.g., the server informing the client when the entity has
      collided with something).

### Optional

- [ ] You might adjust the physical properties of your simulation based on
      proximity to the planetary surface, to represent atmosphere. For example,
      on entering the atmosphere a projectile might begin to slow, turn red and
      possibly break apart. If you do this, ensure there is an option to
      disable the functionality to allow test-firing at the planetary target
      region (see Part A).
- [ ] You might extend the scoring system to relate to accuracy, a higher score
      being awarded for hitting the target region nearer its centre. You might
      also consider having the client-controlled entity (see COMPULSORY
      elements) affect scoring in some way.
- [ ] You might extend the physical representation of the planet beyond a
      sphere, taking into account its geometric complexities in a scalable
      manner.
- [ ] You might implement a high score board which is broadcast by the server
      to the client on termination of the simulation, indicating the top ten
      recorded scores.
- [x] You might introduce broadphase culling to optimise your physics
      computation. More marks will be awarded for more sophisticated
      approaches.
- [ ] You might implement some form of artificial intelligence for your client
      (such as a hierarchical finite state machine) which adjusts the
      client-controlled entity's behaviour based on server messages. For
      example, on learning of a collision in one direction, a client might
      reverse its direction for a period of time, or react in some more
      aggressive fashion.
- [ ] You might implement a debug view, accessed by pressing the 'M' key. This
      would have two modes. On a first press, only the physical representation
      of objects in the environment would be rendered, untextured, using
      coloured lines to indicate connections between vertices against a black
      backdrop.
      On a second press, this 'physics view' would be overlayed on the normal
      graphical environment, to better see how they are connected. On a third
      press, the view would return to normal.

## Submission

YOU MUST SUBMIT THE ZIPPED ARCHIVE OF YOUR CODEBASE TO NESS TWICE - ONCE FOR
PART A, AND ONCE FOR PART B

YOU MUST ATTEND THE COMPULSORY DEMONSTRATION SESSION ON FRIDAY 16th DECEMBER,
BETWEEN 12NOON AND 4PM - IF YOU CANNOT, YOU MUST ARRANGE AN EARLIER
DEMONSTRATION OF YOUR WORK WITH ME PRIOR TO THIS
