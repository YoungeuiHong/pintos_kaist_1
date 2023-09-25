# Scheduling

여러 프로세스를 하나의 (또는 몇 개의) CPU가 처리해야 한다. 어떤 순서로 처리할까?

1. 들어온 순서대로 처리할 수 있다.
2. 처리가 빨리 되는 순서로 처리할 수도 있다.
3. 아니면 다른 우선순위에 따라 처리할 수도 있다.

## 1. 스케줄링의 평가
* 반환 시간: 프로세스의 종료시간 - 프로세스의 도달 시간
* 응답 시간: 프로세스의 시작시간 - 프로세스의 도달 시간

일련의 작업들이 처리될 때, 평균적인 반환시간과 응답시간이 적을수록 좋은 스케줄링이 되겠다.

* 성능: n개의 작업을 얼마나 빨리 처리하느냐
* 공정성: 각각의 작업을 얼마나 균등하게 처리하느냐

예를 들어 만약 작업시간이 빠른 순으로 스케줄링한다면, 오래 걸리는 작업의 경우 그저 대기해야 하는 경우가 있을 수 있다. 이는 각 작업을 공정하게 처리하지 못하고 있는 것으로 볼 수도 있다. 성능과 공정성은 일반적인 스케줄링 정책에서 충돌하는 두 지표일 수 있으며, 양쪽간에 잘 균형을 잡는 것이 미션이 된다. 이를테면, 작업이 대기하고 있는 시간이 길어질수록 우선순위를 높이는 것(aging)도 방법이 될 수 있다.

**👀 선점형/비선점형 스케줄링?**<br/>
* **선점형 preemptive**: 작업이 아직 종료되지 않았어도 필요한 경우 다른 작업으로 context switch가 이루어지는 것.
  * 인터럽트가 나면 switch, 시간(time-slice, tick)이 지나도 switch
  * 현대의 모든 스케줄러는 선점형이다.
* **비선점형 non-preemtive**: 작업이 한 번 시작되고 나면, 종료되거나 인터럽트가 발생할 때까지는 CPU점유를 보장해 주는 것.
  * 인터럽트가 나거나, 작업이 종료되면 switch
  * 예전 일괄처리 시스템이 이런 걸 썼다..

## 2. 스케줄링의 방법

### 2.1. General Scheduling Problems
#### 2.1.1. 선도착선처리 First In First Served
빨리 들어온 작업부터 처리한다. 그렇지만 작업에 소요되는 시간은 고려하지 않기 때문에, 작업 시간이 짧은 여러 작업들이 오래 걸리는 하나의 작업을 기다리고 있는 상황이 벌어질 수 있다. (이것을 convoy effect라고 한다..고 한다.) 이 경우 작업들의 평균 반환 시간과 대기 시간이 늘어난다.

#### 2.1.2. 최단작업우선 Shortest Job First
가장 짧은 시간이 걸리는 작업부터 처리한다. 그런데 모든 작업들이 동시에 들어오는 것은 아니다. 미래에 들어올 작업이 짧다는 것을 어떻게 알 수 있을까? (당연히 모든 작업이 동시에 도착한다고 가정하면 SJF가 최선의 선택이 되겠다.)

#### 2.1.3. 최소잔여시간우선 Shortest Time-to-Completion First
선점형 아이디어를 결합시킨다. 즉, 실행 중인 프로세스의 잔여시간이 새로 들어온 프로세스의 실행시간보다 길면 현재 프로세스를 중단하고 새로운 프로세스를 실행시키는 것이다.

**👀 응답 시간은 새로운 평가기준이다. 왜?**<br/>
반환 시간은 시스템이 나를 얼마나 빨리 처리하느냐? 응답 시간은 시스템이 나에게 얼마나 빨리 응답하느냐. I/O가 중요해짐(현대의 많은 시스템이 대화형)에 따라 시스템이 즉시 응답하는 것이 중요한 스케줄링 평가 기준이 된다. **시분할 컴퓨터**의 등장이 빠른 응답시간의 중요성을 높였는데, 이는 시분할 컴퓨터가 하나의 컴퓨터를 여러 사용자가 동시에 사용하는 것처럼 보이게끔 만들었다는 것과 관련이 있다. 시분할 이전에는 일괄 처리 방식이었는데, 이는 작업을 큐에 모아두고 한번에 처리하는 방식이므로 사용자의 개별 입력마다 즉시 응답을 주기 어려웠다. 하지만 시분할 컴퓨터의 등장으로, 사용자의 요청을 빠르게 처리할 수 있게 되면서 사용자와의 실시간 상호작용이 가능해졌다. (시분할시스템의 구조와 원칙 자체가 여러 사용자와의 실시간 상호작용을 지원할 수 있도록 설계되었다.)

#### 2.1.4. 라운드 로빈 Round-Robin
응답 시간을 높이려면, 프로세스의 실행 시간과 관계없이 일단 들어오면 **빠르게 실행할 수 있도록** 해야 한다. 이후 다음 프로세스에 의해 중단되더라도. 그래서 프로세스마다 동일한 시간 단위 (time-slice)로 나눠서, 이 시간만큼 실행한 후 중단하여 실행 큐의 마지막에 넣고, 실행 큐의 다음 작업으로 전환하는 것을 라운드 로빈 (RR)이라고 한다. 이때의 시간 단위를 time-slice또는 스케줄링 퀀텀 scheduling quantum이라고 한다. 프로세스가 도달한 것 대비 얼마나 빨리 실행되느냐가 중요하므로, time-slice가 짧을수록 응답시간을 최소화하는데는 좋다. 하지만 context switching에 필요한 비용이 증가할 수 있기 때문에 균형을 잘 잡아야 한다.

그런데 또다른 평가 기준인 반환 시간을 생각해보면, 라운드 로빈의 경우 프로세스의 실행시간과 관계없이 동일한 길이로 나누기 떄문에 프로세스 종료까지 시간이 지연된다. (동시에 N개의 프로세스가 다다다.. 온 최악의 경우를 생각해보자.) RR은 공정한 스케줄링이긴 하지만, 이로 인해 전체적인 성능은 낮아질 수 있다. 하지만 그래도 쓰긴 쓴다. 리눅스에 관련값이 있다.

**👀 라운드 로빈 타임 슬라이스의 길이**<br/>
다양한 정핵에 따라 결정된다. 일련의 프로세스나 스레드 단위마다 달라질 수도 있다. 아래는 리눅스에서 기본 설정된 타임 슬라이스 값을 확인하는 명령어 2개. 하지만 모든 프로세스가 100ms단위로 슬라이싱이 이루어지고 있는 것은 아니다. (그리고 타임 슬라이스는 실행의 최대시간임을 기억해야 한다.)
```shell
cat /proc/sys/kernel/sched_rr_timeslice_ms
sysctl kernel.sched_rr_timeslice_ms
// 100
```

**👀 I/O interrupt의 처리**<br/>
하나의 프로세스가 10ms 실행 후 I/O 인터럽트가 발생한다면, 그 시간에 CPU를 다른 프로세스가 점유하도록 하여 연산을 중첩시킬 수 있다. CPU가 결코 쉬지 않게 돌아가도록 하여 이용률을 높임으로서 최적화를 수행한다.

**👀 요약하자면..**<br/>
General Problems에서 알아본 방법들 중 SJF, STCF는 반환시간을 우선하는 알고리즘이었다. (FIFS는 제외하고..) 하지만 프로세스의 응답시간이 늦어진다는 단점이 있다. 반면, RR은 time-slicing으로 인해 최초로 CPU를 점유하는 시간은 짧아지지만, 결국 처리까지 오래 걸린다는 단점이 있다. 이 사이를.. 잘 균형을 잡을 수는 없을까? Time slicing의 장점을 살리면서도 스케줄링의 성능을 높일 수는 없을까? 그리고 무엇보다도, **각 프로세스의 실행 시간을 알 수 없는데 어떻게 스케줄링을 수행할까?** 의 문제가 남아 있다. 이를 Multi Level Feedback Queue로 해결할 수 있다.

### 2.2. Multi-Level Feedback Queue