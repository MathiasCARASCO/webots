La partie 2 de ce TP n'a pas pu être implémentée, pour des raisons de temps.
Toutefois, nous avons essayé d'implémenter deux conteneurs, afin d'abstraire la logique de coordination/inférence par rapport au Serveur Webots.

Pour ce qui est du contrôleur, le Dockerfile ressemble à ceci :

# Use the official Webots image as the base image
FROM cyberbotics/webots:latest

# Set the working directory to /app
WORKDIR /app

# Copy the binary file and source code to the container
COPY khepera_controller /app/khepera_controller
COPY khepera_controller.c /app/khepera_controller.c

# Compile the source code
RUN gcc khepera_controller.c -o khepera_controller

# Set the environment variable for the binary file
ENV KHEPERA_CONTROLLER_BINARY /app/khepera_controller

# Run the binary file when the container starts
CMD ["/app/khepera_controller"]

Il écoute les connexions du client, et exécute les actions sur le robot Khepera.
Le "khepera_controller" est basé sur le fichier "tcpip.c" dans le répertoire "controllers" de l'application Webots, et implémente dans la fonction run() notre logique écrite dans le contrôlleur que nous avons rendu dans ce projet.

Le Dockerfile du client, quand à lui, ressemble à ceci : 

FROM alpine:latest

RUN apk add --no-cache gcc musl-dev

WORKDIR /app

COPY client.c .

RUN gcc -Wall client.c -o client

CMD ["./client"]

Notre client crée simplement une connexion au controlleur, et envoie les commandes en TCP/IP sous forme de chaîne de caractères, en utilisant "l'API" décrite dans le fichier "client.c" d'exemple présent dans le même répertoire que celui cité au-dessus. Typiquement:
 * N: read proximity sensors
 * O: read ambient light sensors
Dans le conteneur, on a donc simplement besoin d'une configuration minimaliste avec alpine, et gcc pour compiler le client, et l'exécuter.