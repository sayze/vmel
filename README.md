# Vmel - Virtual Machine Execution Language

## Overview
Vmel is a domain specific language that simplifies working with remote servers. You get an abstraction of all the boilerplate tasks required to interact with a remote server. We often find the need to deploy applications through a sequence of repititve commands, although there are standardised delployment services out there however they do tend to feel like 'overkill'. Sometimes I might be developing a node app and I simply want to build my packages on a dev server. The steps might be 

1. SSH into the server
2. Pull down repo
3. Run `npm install`
4. Run `npm run build`
5. etc...

The above only provides a fairly trivial but useful set of commands one would want to execute. In truth there are a variety of ways Vmel can be utilised to achieve the desired outcome. Say for instance the below

1. SSH into the server
2. Check if php error log file exists
3. Check if has content
4. Copy the file to local machine

Vmel takes away the complexities of interfacing with a server and offers a wide variety of high level functions to complement this. Most important it offers contextual directory management, so there is no need to manually specifiy full paths when navigating around. For instance if you navigate to `/usr/local` then when the next instruction executes the previous directory will be assumed.
