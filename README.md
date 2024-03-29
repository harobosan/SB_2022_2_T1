# SB_2022_2_T1

Autores
- Gabriel Nazareno Halabi     - 150010290 
- Paulo Mauricio Costa Lopes  - 180112520

----

### Compilar no Linux Bash:

```gcc -o montador montador.cpp -lstdc++```

----

### Executar no Linux Bash:

```./montador -<op> <arquivo>```

----

### Compilar um objeto .exe no Linux Bash:

```x86_64-w64-mingw32-gcc -o montador montador.cpp -lstdc++ -static```

###### Utilizamos o mingw32 para poder criar um .exe que rodasse no Windows

----

### Executar no Windows Powershell:

```.\montador_win2.exe -<op> .\<arquivo>```

----

### Descrição de como rodar no CodeBlocks:

versão/instalador - codeblocks-20.03mingw-setup.exe

#### I - Compiler Flags
 - Settings -> Linker Settings

      *Other linker options:* 

      ```Add -lstdc++ -static```

#### II - Program Args
- Project -> Set programs' arguments

    *Program arguments:*

    ```<op> <arquivo>```

#### III - How to Run
 - Build -> Build and Run 
 - ou pressione F9

----

### Uso:

```
montador <op> <arquivo>
```
###### O nome do arquivo deve ser passado sem especificar a extensão
###### O código automaticamente busca no diretório o arquivo de nome arquivo.asm

### Opções:

```
-p        Gera um arquivo .PRE à partir de um arquivo .asm,
          realizando o pre-processamento do código.

-m        Gera um arquivo .MCR à partir de um arquivo .PRE,
          realizando a substituição de macros no código.

-o        Gera um arquivo .OBJ à partir de um arquivo .MCR,
          montando o código de máquina final após testar por
          erros léxicos, sintáticos e semânticos no código.

-a        Executa em sequência as etapas de pre-processamento,
          análise de macros e consturção do código objeto.

-h        Exibe especificações das opções de linha de comando.
```
