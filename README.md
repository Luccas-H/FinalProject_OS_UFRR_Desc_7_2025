# Simulação e Teste de Driver de Caractere (LKM) para Linux com Uso de Pendrive Bootável e Emuladores  
Universidade Federal de Roraima – Sistemas Operacionais (DCC403)

📌 **Sobre o Projeto**

Este repositório contém a implementação de um driver de caractere (LKM) para Linux que simula um sensor virtual via `/dev/sensor0`, além de tutoriais e exemplos práticos sobre políticas de escalonamento do kernel Linux. O projeto faz parte de uma atividade acadêmica desenvolvida por [Seu Nome], aplicado à coleta de dados via interface serial e análise de desempenho de diferentes políticas de escalonamento.

⚙️ **Funcionalidades**

✅ Módulo de driver de caractere (LKM) simulando sensor virtual  
✅ Leitura e escrita no dispositivo `/dev/sensor0`  
✅ Testes com logs (`dmesg`, `lsmod`, `cat /dev/sensor0`)  
✅ Scripts para automação de carregamento e testes  
✅ Tutorial completo de escalonamento no Linux  
✅ Exemplos de código para manipulação de políticas (FIFO, RR, CFS, DEADLINE)  
✅ Avaliação de desempenho dos escalonadores (wallclock, user, system time)  
✅ Suporte a execução em pendrive bootável ou ambiente emulado (QEMU)

🔧 **Requisitos**

Para compilar, simular e testar o projeto, é necessário ter:

- Distribuição Linux leve (Debian Live, Ubuntu Server, Alpine, etc)
- Ferramentas: gcc, make, kernel headers, kmod (modprobe, insmod, rmmod, lsmod)
- Permissões de superusuário (sudo)
- (Opcional) QEMU para emulação
- Ferramentas para criar pendrive bootável (Ventoy, Rufus, etc)

▶️ **Como Executar**

1. **Clone o repositório:**

    ```sh
    git clone https://github.com/Luccas-H/FinalProject_OS_UFRR_Desc_7_2025.git
    cd FinalProject_OS_UFRR_Desc_7_2025
    ```

2. **Monte o ambiente em um pendrive bootável** (ou use máquina virtual):

    - Instale uma distribuição Linux leve
    - Instale as ferramentas necessárias:

      ```sh
      sudo apt-get update
      sudo apt-get install gcc make linux-headers-$(uname -r) kmod
      ```

    - (Opcional) Instale QEMU:

      ```sh
      sudo apt-get install qemu-system-x86
      ```

3. **Compile o driver:**

    ```sh
    cd driver/
    make
    ```

4. **Carregue o driver:**

    ```sh
    sudo insmod sensor_driver.ko
    sudo mknod /dev/sensor0 c <major> 0
    sudo chmod 666 /dev/sensor0
    ```

    > Substitua `<major>` pelo número major apresentado pelo `dmesg` ou script de build.

5. **Teste o driver:**

    ```sh
    echo "123" > /dev/sensor0
    cat /dev/sensor0
    dmesg | tail
    lsmod | grep sensor_driver
    ```

6. **Remova o driver:**

    ```sh
    sudo rmmod sensor_driver
    ```

7. **Execute avaliações de escalonamento:**

    - Siga os exemplos e scripts em `/escalonamento/` para compilar e rodar testes de desempenho, alterando políticas via `chrt` ou exemplos em C.

📂 **Estrutura do Repositório**


👨‍💻 **Autores**

- Luccas Henrique
- Matheus Garcia

---

Este projeto foi desenvolvido para aplicar, na prática, conceitos de Sistemas Operacionais e Arquitetura de Software, incluindo escrita de módulos do kernel Linux, manipulação de dispositivos virtuais e análise de políticas de escalonamento.

---
