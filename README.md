# ZeOS - Sistema Operatiu (SOA - FIB UPC)

Aquest repositori conté la implementació de **ZeOS**, un sistema operatiu didàctic i senzill dissenyat per a l'arquitectura Intel 80386 (x86). Aquest projecte es desenvolupa com a part de l'assignatura **SOA (Sistemes Operatius Avançats)** a la **FIB - UPC** (Facultat d'Informàtica de Barcelona - Universitat Politècnica de Catalunya).

Actualment, el projecte implementa la totalitat de l'**Entrega 1** i gran part de l'**Entrega 2**, establint una base sòlida per a la gestió de processos, memòria i excepcions.

## 🏗️ Arquitectura del Sistema

El sistema està construït sobre una arquitectura x86 de 32 bits i es divideix en tres blocs principals dins del fitxer binari generat (`zeos.bin`):
1. **Bootloader (`bootsect.S`)**: Sector d'arrencada de 512 bytes que carrega el sistema a memòria.
2. **System Space**: Codi del nucli (kernel) del sistema operatiu, executat amb màxims privilegis.
3. **User Space**: Espai on s'executen els programes d'usuari amb privilegis reduïts.

L'entrada al sistema des de l'espai d'usuari es gestiona a través de la **IDT (Interrupt Descriptor Table)**, permetent transicions segures mitjançant excepcions, interrupcions per maquinari i crides a sistema (System Calls / Fast System Calls via MSR).

## ✨ Funcionalitats Implementades (E1 & E2 Parcial)

S'han desenvolupat diverses funcionalitats clau per al funcionament bàsic del sistema:

### 1. Gestió d'Interrupcions i Excepcions
* **Gestió del Rellotge (Clock)**: Interrupció de temporitzador configurada, actualitzant els "ticks" del sistema.
* **Gestió del Teclat (Keyboard)**: Interrupció per processar les tecles premudes. S'ha implementat un **Circular Buffer** per emmagatzemar i gestionar les pulsacions de forma eficient abans de ser consumides.
* **Excepcions**: Gestió bàsica de les excepcions del sistema (ex. Page Faults, General Protection Faults).

### 2. Crides a Sistema (System Calls)
Implementació de crides al sistema tant per l'esquema clàssic (`int 0x80`) com per **Fast System Calls** (`sysenter`/`sysexit`).
* `write(fd, buffer, size)`: Permet als processos d'usuari imprimir text per pantalla.
* `gettime()`: Retorna el nombre de ticks del sistema des de l'arrencada.
* `getpid()`: Retorna l'identificador del procés actual.
* `fork()`: Crea un procés fill clonat a partir del pare.
* `exit()`: Finalitza i destrueix el procés actual, alliberant els seus recursos.

### 3. Gestió de Processos (Multitasking)
* **Estructures de dades**: Ús de `task_struct` (PCB - Process Control Block) i `task_union` (PCB + pila de sistema acoblats en una sola pàgina de 4KB).
* **Processos Inicials**: Creació i gestió del procés `idle` (PID 0) i el procés `init` (PID 1).
* **Planificador (Scheduler)**: Implementació d'un planificador basat en la política **Round Robin**.
* **Gestió de Cues**: Implementació d'una llista circular doblement enllaçada per a la cua de processos preparats (`readyqueue`) i processos lliures (`freequeue`).
* **Context Switch**: Canvi de context per maquinari/programari guardant l'estat dels registres, canviant l'espai d'adreçament (Paging) i restaurant la pila del nou procés.

### 4. Gestió de Memòria (Paginació)
* Ús de la unitat de gestió de memòria (MMU) del processador i del TLB per traduir adreces lògiques a físiques.
* Compartició de les pàgines del sistema (Codi i Dades) entre tots els processos.
* Les pàgines d'usuari (Dades i Pila) s'assignen de manera privada per a cada procés usant marcs (frames) físics.

## 🛠️ Entorn de Treball i Execució

El sistema operatiu requereix un entorn Linux (ex. Ubuntu) amb les eines adequades (`gcc`, `bin86`) i utilitza **Bochs** com a emulador per x86.

**Compilació i generació de la imatge:**
```bash
make
```

**Compilació amb BOCHS (emulador HW)**
```bash 
make emuldbg
```
