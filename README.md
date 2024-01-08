# Cellular Automata
É uma plataforma TUI de simulação de autómatos celulares.
Automatos inseridos:
- Game Of Life
- Seeds
- Brian's Brain
- Day And Night
- Maze

## Uso
Possue a seguintes fuções:
- `ESPAÇO` para pausar a animação.
- `n` para a próxima geração, quando pausado.
- Use o `MOUSE` para desenhar, quando pausado.
- Seta para direita para mudar qual autómato usar.

## Instalação
```bash
make
```
ou
```bash
gcc termal.c -c -o termal.o && gcc main.c termal.o -o main
```

## Execução
```bash
./main
```
