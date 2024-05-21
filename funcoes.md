Biblioteca: pthread.h

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
-
- thread-> Variável que vai ser utilizada para identificar a thread, tem que ser inicializada com o pthread_t
- attr -> Atributos da thread, se preencher com NULL utiliza os atributos padrão. Até então não lembro de termos definido algum atributo.
- start_routine -> Ponteiro para a função que a thread vai executar.
- arg -> "The thread is created executing start_routine with arg as its sole argument". (EU ACHO QUE, precisa confirmar) São as entradas da função de start_routine

- return int -> A função retorna um inteiro diferente de 0 caso apresente algum erro na criação do thread.

void pthread_exit(void *value_ptr);
-
- The pthread_exit() function shall terminate the calling thread and make the value value_ptr available to any successful join with the terminating thread.
Obs.: Para value_ptr=NULL, a função aguarda todas as threads terminarem, caso seja um valor diferente de NULL a mesma elimina todas as threads.

int pthread_join(pthread_t thread_alvo, void **value_ptr);
-
- Suspende a execução da thread atual, até que a "thread_alvo" seja finalizada.
- Quando pthread_join retorna de forma sucedida, e a chamamos com value_ptr não nulo, o valor passado na pthread_exit() da "thread_alvo" estará disponível no ponteiro value_ptr.

pthread_t pthread_self(void);
-
- The pthread_self() function shall return the thread ID of the calling thread.

int pthread_equal(pthread_t t1, pthread_t t2);
-
- pthread_equal() devolve 0 se as threads forem diferentes e um número maior que zero caso contrário. 

Criação de mutexes:
-
Estática:
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

Dinâmica:
pthread_mutex_t mymutex;
...
pthread_mutex_init(&mymutex,NULL);

int pthread_mutex_destroy(pthread_mutex_t *mutex_alvo);
-
- Destruição do mutex_alvo.

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
-
- Criação do mutex, mutex deve ser inicializado com o tipo pthread_mutex_t.
- attr são os atríbutos, utilize NULL para inicializar com o padrão.
- Se bem sucedida, as funções pthread_mutex_init() e pthread_mutex_destroy() retornarão zero, caso contrário retornarão um código de erro;

int pthread_mutex_lock(pthread_mutex_t *mutex);
-
- A função tenta bloquear o mutex, caso o mesmo já esteja bloqueado o thread atual será bloqueado até que o mutex fique livre.

int pthread_mutex_trylock(pthread_mutex_t *mutex);
-
- A função tenta bloquear e retorna 0 caso bem sucedido, caso contrário retornar um valor diferente de 0 e continua a execução do thread atual.

int pthread_mutex_unlock(pthread_mutex_t *mutex);
-
- A função desbloqueia o mutex.

Criação e destruição de variáveis de condição
-

Estática:
pthread_cond_t mycv = PTHREAD_COND_INITIALIZER;

Dinâmica:
pthread_cond_t mycv;
...
pthread_cond_init(&mycv,NULL);

Destruição:
pthread_cond_destroy(&mycv)

Usando variáveis de condião
-
- Esperar que certa condição torne-se verdadeira:
- pthread_cond_wait(cond_var, mutex);
- Avisar outra(s) thread(s) que a condição tornou-se verdadeira:
-  pthread_cond_signal(cond_var);
-  pthread_cond_broadcast(cond_var);
-  Signal avisa a apenas uam thread, enquanto broadcast a todas vinculadas à condição.

pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *restrict attr, unsigned count);
-
- Inicializa uma barreira, mecanismo de sincronia entre várias threads.
- barrier -> Variável vinculada a barreira, deve ser inicializado com o tipo pthread_barrier_t.
- attr -> Atributos, se NULL faz os atríbutos padrão.
- count -> Quantia de threads necessária para atingir o alvo.
- Retorna 0 caso sucesso, e um valor diferente caso apresente algum erro.

int pthread_barrier_wait(pthread_barrier_t *barrier);
-
- Aguarda a quantia count de threads chegarem nesse wait, quando chegar libera todos os threads.

pthread_barrier_destroy(pthread_barrier_t *barrier_alvo);
-
- Destrói a barrier_alvo


