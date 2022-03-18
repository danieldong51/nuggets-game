# CS50 Design review
## Team of 4: team-tux

### Total (41/50)

#### Presentation (12/12 points)

"In 10 minutes you must present an *overview* of your design decisions and how your group will divide the work; the design document provides more detail. Be prepared to answer questions and receive feedback."

* (4/4) Good overview of the design.
* (4/4) Decomposition and data structures.
* (4/4) Plan for division of the work.

#### Document (29/38 points)

"Your design document (written in Markdown) shall describe the major design decisions, plan for testing, and the roles of each member of your group."

**Client (8/14):**

* (4/4) User interface
* (1/2) Inputs and outputs
  - communication with the server is **not** through `stdin`
* (1/2) Functional decomposition into functions/modules
  - you must describe the funtions here in the design document - not just referring to requirements
* (2/2) Major data structures
  - no language-specific (C) code in the design file.
* (0/2) High-level pseudo code (plain English-like language) for logic/algorithmic flow
* (0/2) Testing plan, including unit tests, integration tests, system tests

**Server (21/24):**

* (4/4) User interface
  - you should at least _show_ the command line here, like was done above for client.  
* (4/4) Inputs and outputs
* (4/4) Functional decomposition into functions/modules
* (3/4) Major data structures
  - The server and client are separate programs, so the _game_ struct cannot be accessible to both.  
* (2/4) High-level pseudo code (plain English-like language) for logic/algorithmic flow
  - Only done for server main - what about all the functions/modules ?
* (4/4) Testing plan, including unit tests, integration tests, system tests
