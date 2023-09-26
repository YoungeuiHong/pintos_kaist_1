<small></small>

OS
* 만드는 과정: Design Knowledge
* 만든 산출물: Knowledge
운영체제의 공부는 top down식으로 해야 한다. 어떠한 요구사항이 있어서, 어떤 방식으로 무엇이 만들어졌는지를 알아야 한다. 예를 들면 스레드의 설계 이유. 이것이 시스템 과목의 설계 이유다. Knowledge에 대한 설명은 책에 많이 있다. 그렇지만 설계에 대한 지식은 많이 없다. 그래서 여기에 집중해서 강의할 것.

동작을 잘 이해하기 위해 운영체제를 배운다. 운영체제의 기본적인 탄생의 이유는. 하드웨어의 복잡한 기능들을 감춰주는 것이 운영체제의 일차적인 목적이다. 이 감추는 방법들이 어떻게 동작하고 있는지를 알아야 한다.

All problems in Computer Science can be solved by another level of indirection.

전산학의 모든 문제는 또 다른 수준의 간접층으로 해결할 수 있다.

하나의 레벨과 다른 레벨을 매핑시킨다.. 페이징 (가상주소와 물리주소)
이걸 어떻게 효과적으로 만들까? 파일 시스템은 가상메모리와 유사한 방법의 level of indirection을 사용한다. 그런데 얘는 매핑 function으로 (페이징이 아닌) 인덱싱을 사용하고, 이걸 소프트웨어적으로 구현한다. 이게 가능한 이유는 스토리지 (하드디스크)의 속도가 느리고, 비휘발성 메모리이기 때문이다. 인덱싱의 기법은 (대체로 통일된 페이징 기법과는 달리) 현재로서도 100여가지가 넘는다.

인덱싱 기법에는 indirect index, file allocation table, extent tree, b tree, radix tree 등이 있다.
* 파일을 abstract한 것이 리눅스에서는 inode이다.
* 파일을 저장할 때 스토리지 블록을 할당한다.
* 파일을 쓸 때는 DRAM으로 캐시된다? 그러다가 저장을 누르는 순간 스토리지 블록으로 간다.
* 이 파일은 항상 crash consistent하게 쓰고 저장되어야 한다. (메모리 - 디스크)

소프트웨어를 개발하는 사람으로서, OS를 왜 배울까?
* 추상화 기법을 이해할 수 있다.
* 웹 서버를 만든다고 생각해보자, 하나의 유저를 하나의 커넥션으로 추상화할 수 있다.
* 유저 간의 프로텍션을 제공하고 싶다. 프로세스 또는 스레드로 나눈다.