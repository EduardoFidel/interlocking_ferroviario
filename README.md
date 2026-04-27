# Sistema de Interbloqueo Ferroviario Inteligente

Este proyecto implementa un sistema de control de tráfico ferroviario automatizado utilizando Arduino. El sistema gestiona el paso de trenes en un sector de vía única compartida, garantizando la seguridad mediante sensores ultrasónicos, señalización visual/auditiva y un cambio de vía motorizado.

## 📋 Características Principales
* [cite_start]**Gestión de Prioridades:** Detecta qué tren llega primero a la zona de control (30 cm) y le otorga el paso automáticamente[cite: 66, 67].
* [cite_start]**Sistema de Interbloqueo:** Bloquea la vía contraria mientras una está activa para prevenir colisiones[cite: 50, 52].
* [cite_start]**Alerta de Intrusión:** Si un tren ignora la señal de alto y se acerca a menos de 10 cm, se activa una alarma sonora y un aviso en la pantalla LCD[cite: 71, 72, 76, 77].
* [cite_start]**Interfaz de Usuario:** Pantalla LCD de 16x2 que muestra el estado de las vías en tiempo real y semáforos LED (Rojo/Verde)[cite: 48, 49, 58].

## 🛠️ Configuración de Hardware
[cite_start]El sistema utiliza los siguientes pines del Arduino (definidos en el código)[cite: 41]:

| Componente | Pin Arduino | Descripción |
| :--- | :---: | :--- |
| **Sensor A (Ultrasonido)** | TRIG: 2 / ECHO: 3 | Detección de tren en Vía A |
| **Sensor B (Ultrasonido)** | TRIG: 4 / ECHO: 5 | Detección de tren en Vía B |
| **Semáforo Vía A** | Verde: 6 / Rojo: 7 | Indicadores visuales Vía A |
| **Semáforo Vía B** | Verde: 10 / Rojo: 11 | Indicadores visuales Vía B |
| **Servo Motor** | 9 | Control de posición física de la vía |
| **Buzzer** | 8 | Alarma de intrusión y avisos |
| **Pantalla LCD** | SDA/SCL (I2C) | Visualización de mensajes de estado |

## ⚙️ Lógica de Operación
[cite_start]El software se basa en una **Máquina de Estados Finitos (FSM)**[cite: 43]:
1.  **IDLE:** Estado de espera. [cite_start]Ambas vías están bloqueadas hasta que se detecta un tren a < 30 cm[cite: 53, 54, 62, 64].
2.  [cite_start]**VIA_A_ACTIVA:** El servo se mueve a 30°, habilitando el paso para la Vía A[cite: 49].
3.  [cite_start]**VIA_B_ACTIVA:** El servo se mueve a 150°, habilitando el paso para la Vía B[cite: 51].
4.  [cite_start]**Detección de Conflicto:** Si ambos sensores detectan presencia simultáneamente, el sistema resuelve la prioridad basándose en el *timestamp* de la primera detección[cite: 66, 67, 68].

## 🚀 Instalación y Uso
1.  **Estructura de Carpetas:** Asegúrate de que el archivo `.ino` esté dentro de una carpeta con su mismo nombre (esto responde al punto 2 de las sugerencias):
    ```text
    /interlocking_ferroviario
    └── interlocking_ferroviario.ino
    ```
2.  **Librerías Necesarias:** Instala desde el Gestor de Librerías de Arduino:
    * `Servo`
    * `Adafruit_LiquidCrystal`
3.  **Carga:** Conecta tu Arduino y sube el código.

## 📚 Materia
**Curso:** Desarrollo Adaptativo e Integrado de Software
