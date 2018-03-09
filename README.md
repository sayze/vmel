# Vmel - Virtual Machine Execution Language

## Overview
Vmel is a domain specific language that simplifies working with remote servers. You get an abstraction of all the boilerplate tasks required to interact with a remote server. We often find the need to deploy applications through a sequence of repititve commands, although there are standardised delployment services out there however they do tend to feel like 'overkill'. Sometimes I might be developing a node app and I simply want to build my packages on a dev server. The steps might be 

1. SSH into the server
2. Pull down repo
3. Run `npm install`
4. Run `npm run build`
5. etc...

Vmel takes away the chore of worrying about how you interface with your server and offers a wide variety of high level functions to complement this. Most important it offers contextual directory management, so there is no need to manually specifiy full paths when navigating around.

## Implementation
Vmel is an interpreted language which translates source code into C.
