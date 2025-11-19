/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;

//classe runnable
class MyCallable implements Callable<Long> {
    // construtor
    MyCallable() {
    }

    // método para execução
    public Long call() throws Exception {
        long s = 0;
        for (long i = 1; i <= 100; i++) {
            s++;
        }
        return s;
    }
}

// Classe Primo que implementa Callable para verificação assíncrona de
// primalidade
class Primo implements Callable<Boolean> {
    private long n;

    public Primo(long n) {
        this.n = n;
    }

    // Método para determinar se um número é primo
    private boolean ehPrimo(long n) {
        if (n <= 1)
            return false;
        if (n == 2)
            return true;
        if (n % 2 == 0)
            return false;
        for (long i = 3; i <= Math.sqrt(n); i += 2) {
            if (n % i == 0)
                return false;
        }
        return true;
    }

    // Método executado assincronamente que retorna se o número é primo
    public Boolean call() throws Exception {
        return ehPrimo(n);
    }
}

// classe do método main
public class FutureHello {
    private static final long N = 10_000_000; // Pode ser um valor bastante grande

    // Método para verificar se um número é primo (versão sequencial)
    private static boolean ehPrimo(long n) {
        if (n <= 1)
            return false;
        if (n == 2)
            return true;
        if (n % 2 == 0)
            return false;
        for (long i = 3; i <= Math.sqrt(n); i += 2) {
            if (n % i == 0)
                return false;
        }
        return true;
    }

    // Método para contar primos de forma sequencial (para validação)
    private static long contarPrimosSequencial(long n) {
        long contador = 0;
        for (long i = 1; i <= n; i++) {
            if (ehPrimo(i)) {
                contador++;
            }
        }
        return contador;
    }

    public static void main(String[] args) {
        // Verifica se o número de threads foi passado como argumento
        int nthreads = 10; // valor padrão
        if (args.length > 0) {
            try {
                nthreads = Integer.parseInt(args[0]);
                if (nthreads <= 0) {
                    System.out.println("Numero de threads deve ser maior que zero. Usando valor padrao: 10");
                    nthreads = 10;
                }
            } catch (NumberFormatException e) {
                System.out.println("Argumento invalido. Usando valor padrao de threads: 10");
            }
        }
        System.out.println("Configuracao: N = " + N + ", NTHREADS = " + nthreads);

        System.out.println("\n=== VERSAO SEQUENCIAL ===");
        long inicioSeq = System.currentTimeMillis();
        long primosSequencial = contarPrimosSequencial(N);
        long fimSeq = System.currentTimeMillis();
        System.out.println("Quantidade de numeros primos (sequencial): " + primosSequencial);
        System.out.println("Tempo de execucao: " + (fimSeq - inicioSeq) + " ms");

        System.out.println("\n=== VERSAO CONCORRENTE (Futures) ===");
        long inicioCon = System.currentTimeMillis();

        // cria um pool de threads (NTHREADS)
        ExecutorService executor = Executors.newFixedThreadPool(nthreads);
        // cria uma lista para armazenar referencias de chamadas assincronas
        List<Future<Boolean>> list = new ArrayList<>();

        // Submete tarefas assíncronas para verificar primalidade de 1 até N
        for (long i = 1; i <= N; i++) {
            Callable<Boolean> worker = new Primo(i);
            /*
             * submit() permite enviar tarefas Callable ou Runnable e obter um objeto Future
             * para acompanhar o progresso e recuperar o resultado da tarefa
             */
            Future<Boolean> submit = executor.submit(worker);
            list.add(submit);
        }

        System.out.println("Tarefas submetidas: " + list.size());
        // pode fazer outras tarefas...

        // recupera os resultados e conta quantos números primos foram encontrados
        long contadorPrimos = 0;
        for (Future<Boolean> future : list) {
            try {
                if (future.get()) { // bloqueia se a computação nao tiver terminado
                    contadorPrimos++;
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (ExecutionException e) {
                e.printStackTrace();
            }
        }
        long fimCon = System.currentTimeMillis();
        System.out.println("Quantidade de numeros primos (concorrente): " + contadorPrimos);
        System.out.println("Tempo de execucao: " + (fimCon - inicioCon) + " ms");

        System.out.println("\n=== VALIDACAO ===");
        if (primosSequencial == contadorPrimos) {
            System.out.println("CORRETO! Ambas as versoes encontraram " + contadorPrimos + " primos.");
        } else {
            System.out.println("ERRO! Resultados diferentes:");
            System.out.println("  Sequencial: " + primosSequencial);
            System.out.println("  Concorrente: " + contadorPrimos);
        }

        executor.shutdown();
    }
}
