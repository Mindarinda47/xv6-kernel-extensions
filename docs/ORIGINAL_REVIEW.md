# Original Implementation Review

2023년 학부 구현과 이후 정책 수정 범위를 구분합니다.

## 확인된 원본 구현

### System call

- `getppid` 시스템 콜 번호와 dispatcher 등록
- 사용자 API와 assembly stub 연결
- 현재 프로세스의 부모 PID 반환

### Scheduler

- 프로세스별 nice 값 저장
- 기본 nice 20, 허용 범위 `[0, 40)`
- fork 시 nice 상속
- 낮은 nice 우선 선택
- 동일 nice에서 RUNNABLE 전환 순번을 이용한 FCFS
- `yield`, `setnice`, `getnice` 시스템 콜
- xv6 CPU 수를 1로 제한

### Lazy allocation

- `sbrk`에서 즉시 물리 페이지를 할당하지 않고 `sz`만 증가
- page fault address를 page boundary로 내림
- 물리 페이지 할당 및 0 초기화
- 사용자 쓰기 가능 페이지로 매핑

## 이후 정책 점검

기존 코드의 타이머 인터럽트 경로가 과제의 비선점 정책과 다르게 `yield()`를 호출했습니다.
수정 과정에서는 ticks 갱신과 wakeup 처리는 유지하고, 타이머에 의한 자동
CPU 양보만 제거했습니다. 명시적 `yield`와 `setnice` 이후의 재선택 경로는 유지했습니다.

주소 검증, 메모리 부족 처리와 sparse address space 복사는 과제 핵심 범위와 구분되는
운영체제 확장 고려사항으로 정리했습니다.

## 포트폴리오에서의 표현 원칙

- 2023년 구현과 2026년 개선을 별도로 표시합니다.
- 제공 테스트를 본인 작성물로 표시하지 않습니다.
- 실행하지 않은 테스트를 통과했다고 표시하지 않습니다.
- 원본 구현과 이후 수정 사항을 구분합니다.
