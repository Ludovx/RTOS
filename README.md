# RTOS
Projet-TP de RTOS

# Commentaires sur le code
Pour effectuer les actions demandées, on utilise des tâches. 

Pour envoyer des données d'une tâche à l'autre, on utilise des queues. Pour envoyer une donnée, une tâche la place dans la queue et pour la récupérer, il faut la sortir de la queue. 

Pour sécuriser l'accès au port série, on utilise des semaphores. Ils permettent de réserver le port à la tâche en cours.
