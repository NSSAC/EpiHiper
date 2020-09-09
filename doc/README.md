# EpiHiper Server Mode

## Abstract
The EpiHiper Server Mode will allow the simulation process to be controlled by an external program. EpiHiper will report the summary ouput back after the initiation and after each simulation step. Instructions can be sent to EpiHiper on how to continue after each step.

## Instructions
A list of instruction which can be given to EpiHiper after each step (including initialization):
* __reset:__ Reset the state to just after the initialization
* __apply:__ Apply the a list of intervention to the current state
* __continue:__ Continue the simulation
* __stop:__ Stop simulation and shut down server.

EpiHiper will return after these commands have been executed

## Implementation
The communication will be through MPI. A detailed explanation of the process can be found [here](https://www.mpi-forum.org/docs/mpi-2.0/mpi-20-html/node100.htm). After EpiHiper has been started and complete initialization it will initiate communication through:
```
int MPI_Open_port(MPI_Info info, char *port_name)
int MPI_Comm_accept(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)
```
After the communication is established EpiHiper will accept the above instructions. 

The client will establis communication through:
```
int MPI_Comm_connect(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm)
```

The instructions will be given to EpiHiper as JSON encoded string in the following format:
```
{
  "command": ["reset"|"apply"|"continue"|"stop"],
  "argument": {
    // A valid EpiHiper initialization file
  }
}
```  
The argument is only required for the __apply__ instruction. Other insturctions currently do not except argument.

EpiHiper will return buffer of `double` in the same order as in the summary output.