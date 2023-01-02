# SB_2022_2_T1

Autores
- Gabriel Nazareno Halabi     - 150010290 
- Paulo Mauricio Costa Lopes  - 180112520

 ----


Compilar no Linux Bash:

  ```gcc -o montador montador.cpp -lstdc++```
  
  ----

Executar no Linux Bash:

  ```./montador -<op> <arquivo_fonte>```
  
  ----

Compilar um objeto .exe no Linux Bash:

  ```x86_64-w64-mingw32-gcc -o montador montador.cpp -lstdc++ -static```
  
  ###### Utilizamos o mingw32 para poder criar um .exe que rodasse no Windows

----

Executar no Windows Powershell:

  ```.\montador_win2.exe -<op> .\<arquivo_fonte>```


Uso:      montador -<op> <arquivo_fonte>
Opções:
          -p        Gera um arquivo .PRE à partir de um arquivo .asm,
                    realizando o pre-processamento do código.
 
          -m        Gera um arquivo .MCR à partir de um arquivo .PRE,
                    realizando a substituição de macros no código.
 
          -o        Gera um arquivo .OBJ à partir de um arquivo .MCR,
                    montando o código de máquina final após testar por
                    erros léxicos, sintáticos e semânticos no código.
 
          -h        Exibe especificações das opções de linha de comando.
