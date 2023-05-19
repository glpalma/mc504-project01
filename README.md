# Problema da pizzaria

## Contextualização

Em uma pizzaria em Barão Geraldo, existe uma pizzaria chamada Pizza504 que começou a fazer muito sucesso após a pandemia. Graças a esse crescimento, o número de pedidos começou a aumentar e agora os pizzaiolos estão sempre ocupados fazendo pizza. No entanto, surge um problema: o número de fornos é limitado, e os pizzaiolos sempre se confundem ao tentar usar fornos que já estão sendo utilizados por outros funcionários, e isso causa uma grande confusão.

<!-- Em uma pizzaria em Barão Geraldo, existem dois sabores de pizza: com queijo e sem queijo. Porém, existe um problema nessa pizzaria: os pizzaiolos sempre se esbarram e encontram grandes dificuldades de se organizarem quando mais de um precisa pegar o mesmo ingrediente. Portanto, esse projeto visa resolver esses conflitos utilizando semáforos para sincronizar a utilização de ingredientes pelos pizzaiolos, sem que eles se esbarrem ou tentem utilizar ingredientes que já estão em uso por outros colegas. -->

## Checklist
(x) Implementar funcionamento básico da thread do forno; \
(x) Planejar funcionamento básico da thread do pizzaiolo;\
(x) Fazer lista de estados assumidos pelo pizzaiolo; \
( ) Implementar comunicação entre forno e pizzaiolo para indicar o progresso de cocção da pizza; \
( ) Pensar como a movimentação do pizzaiolo vai ser representada; \
(x) Planejar animação do projeto \
(x) Implementar animação do projeto

## Lista de melhorias
(x) Quando um pizzaiolo terminar de assar sua pizza, tem que voltar pro final da fila (deixar outros assarem tb) -> um sleep foi adicionado  
(-) Planejar alguma forma de separar o funcionamento dos fornos dos pizzaiolos -> achamos uma mudança desnecessária
(x) Aumentar o espaço entre os pizzaiolos
(x) Implementar animação do repositor
(x) Resolver problema do repositor aparecer abastecendo vários fornos ao mesmo tempo -> foi adicionada uma proteção no vetor que guarda quem está abastecendo cada forno
( ) Depois que o repositor sai, o gás demora a atualizar em alguns casos
( ) Resolver problema de a animação inteira travar quando acontece abastecimento (por causa dos semáforos de proteção talvez?)