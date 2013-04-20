Freepoint
=========

Freepoint is intended to be a client-side implementation of a
sharepoint-like system that I envision.

The overall idea is to replace Access applications linked with
sharepoint tables the reason for this is that Sharepoint is a very
closed and clunky set of tools not to mention to abhorrent liscencing
that comes with it.

Freepoint has a choice of C++ or Python clients. This keeps me honest
about the API.

Server-Side
-----------

* Basic Authentication (real basic)
  - Clients authenticate using username/password combinations, which
	they are then rewarded with a session token which is supposed to
	be supplied with on each subsequent request.
  - Passwords are salted and stored with SHA512.
* Data retrieval
* Data updating
* JSON Marshaling

Client-side
-----------

* Open/View tables
* Edit data
* Create tables
* Manage connections


TODO:
-----

* Figure out how to create Sphinx docs with Qt
* Make the database creator more featureful
