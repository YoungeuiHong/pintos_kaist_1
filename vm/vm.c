/* vm.c: Generic interface for virtual memory objects. */

#include "vm/vm.h"
#include "include/threads/mmu.h"
#include "include/threads/vaddr.h"
#include "threads/malloc.h"
#include "vm/inspect.h"
#include <string.h>
/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */

void vm_init(void) {
  vm_anon_init();
  vm_file_init();
#ifdef EFILESYS /* For project 4 */
  pagecache_init();
#endif
  register_inspect_intr();
  /* DO NOT MODIFY UPPER LINES. */
  /* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type page_get_type(struct page *page) {
  int ty = VM_TYPE(page->operations->type);
  switch (ty) {
  case VM_UNINIT:
    return VM_TYPE(page->uninit.type);
  default:
    return ty;
  }
}

/* Helpers */
static struct frame *vm_get_victim(void);
static bool vm_do_claim_page(struct page *page);
static struct frame *vm_evict_frame(void);

/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
/* 초기화 프로그램을 사용하여 보류 중인 페이지 객체를 생성합니다. 페이지를
 * 직접 생성하려면이 함수 또는 `vm_alloc_page`를 통해 만들지 마십시오. */
// 이 함수의 목적은 주어진 가상 주소(`upage`)에 대한 페이지를 생성하고, 이를
// 보조 페이지 테이블에 추가하는 것입니다.
bool vm_alloc_page_with_initializer(enum vm_type type, void *upage,
                                    bool writable, vm_initializer *init,
                                    void *aux) {
  // VM 타입이 초기화되었는지 확인
  ASSERT(VM_TYPE(type) != VM_UNINIT)

  // 현재 스레드의 보조 페이지 테이블에 대한 포인터 획득
  struct supplemental_page_table *spt = &thread_current()->spt;

  /* Check wheter the upage is already occupied or not. */
  if (spt_find_page(spt, upage) == NULL) {

    /* TODO: Create the page, fetch the initialier according to the VM type,
     * TODO: and then create "uninit" page struct by calling uninit_new. You
     * TODO: should modify the field after calling the uninit_new. */
    /* TODO: Insert the page into the spt. */

    /* TODO: 페이지를 생성하고 VM 유형에 따라 초기화 프로그램을 가져와서
     * TODO: "uninit" 페이지 구조체를 uninit_new를 호출하여 생성합니다.
     * TODO: uninit_new를 호출한 후 필드를 수정해야 합니다. */
    /* TODO: 페이지를 보조 페이지 테이블에 삽입합니다. */

    struct page *newpage = (struct page *)calloc(1, sizeof(struct page));
    // if (newpage == NULL) {
    //   return false;
    // }

    void *round_page = pg_round_down(upage);

    switch (type) {
    case VM_ANON:
      uninit_new(newpage, round_page, init, type, aux, anon_initializer);

      break;
    case VM_UNINIT:
      uninit_new(newpage, round_page, init, type, aux, NULL);
      break;
    case VM_FILE:
      uninit_new(newpage, round_page, init, type, aux, NULL);
      break;
    default:
      // free(newpage);
      // return false;
      break;
    }
    if (!spt_insert_page(spt, newpage)) {
      // free(newpage);
      return false;
    }
    return true;
  }
  // return true;
err:
  return false;
}

/* Find VA from spt and return page. On error, return NULL. */
/* 인자로 넘겨진 보조 페이지 테이블에서로부터 가상 주소(va)와 대응되는 페이지
 * 구조체를 찾아서 반환합니다. 실패했을 경우 NULL를 반환합니다.
 */
struct page *spt_find_page(struct supplemental_page_table *spt UNUSED,
                           void *va UNUSED) {
  // struct page *page = NULL;
  /* TODO: Fill this function. */

  if (spt == NULL || va == NULL) // 인자가 제대로 주어졌는지 예외 처리
    return NULL;
  struct page *page = (struct page *)malloc(sizeof(struct page));
  page->va = pg_round_down(va);

  // page.va = va;
  // hash_find함수로 찾고 싶은 hash_elem을 검색해서 반환
  struct hash_elem *e = hash_find(&spt->hash_pt, &page->hash_elem);
  free(page);
  // 위 hash_elem을 이용해서 원하는 page 구조체를 찾음
  // struct page *page = hash_entry(e, struct page, hash_elem);
  if (e == NULL) // 찾은 page가 NULL이라면 예외 처리
    return NULL;

  // return page;
  return hash_entry(e, struct page, hash_elem);
}

/* Insert PAGE into spt with validation. */
/* 인자로 주어진 보조 페이지 테이블에 페이지 구조체를 삽입합니다. 이 함수에서
 * 주어진 보충 테이블에서 가상 주소가 존재하지 않는지 검사해야 합니다. */
bool spt_insert_page(struct supplemental_page_table *spt UNUSED,
                     struct page *page UNUSED) {
  int succ = false;
  /* TODO: Fill this function. */

  if (spt_find_page(spt, page->va) == NULL) {
    hash_insert(&spt->hash_pt, &page->hash_elem);
    succ = true;
  }
  return succ;
}

void spt_remove_page(struct supplemental_page_table *spt, struct page *page) {
  vm_dealloc_page(page);
  return true;
}

/* Get the struct frame, that will be evicted.
 * 페이지 대체 대상이 될 구조체 프레임을 반환합니다. */
static struct frame *vm_get_victim(void) {
  struct frame *victim = NULL;
  /* TODO: The policy for eviction is up to you. */

  return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.
 * 하나의 페이지를 대체하고 대응되는 프레임을 반환합니다.
 * 에러 발생 시 NULL을 반환합니다. */
static struct frame *vm_evict_frame(void) {
  struct frame *victim UNUSED = vm_get_victim();
  /* TODO: swap out the victim and return the evicted frame. */
  // swap_out(victim->page);
  return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
/* palloc()와 프레임을 가져옵니다. 사용 가능한 페이지가 없으면 페이지를
 * 대체하고 반환합니다. 이 함수는 항상 유효한 주소를 반환합니다. 즉, 사용자 풀
 * 메모리가 가득 찬 경우, 이 함수는 사용 가능한 메모리 공간을 얻기 위해 프레임을
 * 대체합니다. */
static struct frame *vm_get_frame(void) {
  struct frame *frame = (struct frame *)malloc(sizeof(struct frame));
  frame->kva = palloc_get_page(PAL_USER | PAL_ZERO);
  if (frame == NULL || frame->kva == NULL) {
    free(frame);
    PANIC("todo");
    // return false;
  }
  // list_push_back(&frame_list, &frame.frame_elem);
  // 할당된 프레임을 초기화
  frame->page = NULL; // 페이지 연결 초기화
  // frame->kva = NULL;  // 가상 주소 초기화

  /* TODO: Fill this function. */
  // 수도코드:
  // 1. 프레임 할당을 시도 (palloc)
  // 2. 할당된 프레임이 있을 경우 할당된 프레임을 반환
  // 3. 할당된 프레임이 없을 경우 페이지 대체 알고리즘을 사용해 프레임 대체
  // (evict)
  // 4. 대체된 프레임을 반환

  ASSERT(frame != NULL); // 프레임이 할당되지 않은 경우 예외 처리
  ASSERT(frame->page == NULL); // 프레임에 이미 페이지가 연결된 경우 예외 처리
  return frame;
}

/* Growing the stack. */
static void vm_stack_growth(void *addr UNUSED) {
  // struct thread *curr = thread_current();
  // void *round_va = pg_round_down(addr);

  // VM_ANON 타입의 페이지를 할당하여 스택을 확장합니다.
  vm_alloc_page(VM_ANON, addr, true);
  // struct page *page = spt_find_page(&curr->spt, round_va);
  // vm_claim_page(page);
}

/* 쓰기 보호 페이지에서 발생하는 페이지 폴트 처리 (Handle the fault on
 * write-protected page) */
static bool vm_handle_wp(struct page *page UNUSED) {
  // 주어진 페이지를 처리하기 위한 코드를 구현합니다.
  // 이 함수는 쓰기 보호 페이지에서 발생하는 페이지 폴트를 처리합니다.
  // 필요한 작업에 따라 구현이 필요합니다.
  // page 매개변수는 처리해야 하는 페이지를 나타냅니다.
  // 이 함수는 해당 페이지에 대한 처리 작업을 수행하도록 구현되어야 합니다.
  // 필요한 경우 추가 코드를 작성합니다.
}

/* Return true on success */
bool vm_try_handle_fault(struct intr_frame *f UNUSED, void *addr UNUSED,
                         bool user UNUSED, bool write UNUSED,
                         bool not_present UNUSED) {
  struct supplemental_page_table *spt UNUSED = &thread_current()->spt;
  void *round_va = pg_round_down(addr);
  struct page *page = spt_find_page(spt, round_va);

  // 주어진 주소가 유저 영역 내에 있는지 확인합니다.
  if (addr < USER_STACK && addr > KERN_BASE)
    return false;
  // if (page->writable == false && write == true)
  //   return false;

  // 유저 스택의 시작과 끝을 계산합니다.
  void *stack_start = USER_STACK;
  void *stack_end = stack_start - (1 << 20);

  // 주어진 주소를 페이지 경계로 반올림하여 정렬합니다.

  if (page == NULL) {
    // 페이지가 페이지 테이블에 없는 경우, 스택 확장이 필요한지 확인합니다.
    if (round_va >= stack_end && round_va < stack_start &&
        f->rsp == (uint64_t)addr) {
      // 스택 확장 함수를 호출하여 스택을 확장합니다.
      vm_stack_growth(round_va);
    } else {
      return false;
    }
    // 스택 확장 후, 페이지를 다시 검색합니다.
    // if (page != NULL) {
    //   if (page->writable == false && write == true)
    //     return false;
    // }else
    //   return false;
  }
  page = spt_find_page(spt, round_va);

  return vm_do_claim_page(page);
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void vm_dealloc_page(struct page *page) {
  destroy(page);
  free(page);
}

/* Claim the page that allocate on VA. */
/* 주어진 가상 주소(va)에 해당하는 페이지를 할당하고, 해당 페이지를 사용 가능한
 * 물리 메모리 프레임에 매핑하는 역할을 합니다. */
bool vm_claim_page(void *va UNUSED) {
  /* TODO: Fill this function */
  /* TODO: 여기서는 주어진 가상 주소(va)에 해당하는 페이지를 어떻게 찾을 것인지
   * 구현해야 합니다. */
  struct thread *curr = thread_current();
  struct page *page = spt_find_page(&curr->spt, va);

  if (page == NULL) // 해당하는 페이지를 찾지 못했을 경우 예외 처리
    return false;

  // 페이지를 실제로 할당하고 MMU 설정
  return vm_do_claim_page(page);
}

/* Claim the PAGE and set up the mmu. */
/* 페이지를 할당하고 MMU(Memory Management Unit)를 설정합니다.

페이지를 물리 프레임에 할당한 후, 페이지 테이블 엔트리를 업데이트하여 해당
페이지의 가상 주소와 물리 주소 간의 매핑을 설정합니다. 이렇게 하면 페이지가 실제
물리 메모리에 매핑되고, 프로세스가 해당 페이지에 접근할 수 있게 됩니다. */
static bool vm_do_claim_page(struct page *page) {
  /* 프레임을 할당합니다. */
  struct frame *frame = vm_get_frame();

  /* Set links */
  /* 프레임과 페이지를 서로 연결합니다. */
  frame->page = page;
  page->frame = frame;

  struct thread *curr = thread_current();

  /* TODO: Insert page table entry to map page's VA to frame's PA. */
  /* TODO: 페이지 테이블 엔트리를 설정하여 페이지의 VA를 프레임의 PA에
   * 매핑합니다. */
  uint64_t *my_pml4 = curr->pml4;

  void *va = page->va;
  void *pa = frame->kva;

  if (!pml4_set_page(my_pml4, page->va, frame->kva, true)) {
    frame->page = NULL; // 페이지 연결 해제
    page->frame = NULL; // 프레임 연결 해제
    return false;
  }

  /* 페이지를 스왑인(swap in)하고 페이지를 프레임의 KVA로 복사합니다. */
  return swap_in(page, frame->kva);
}

/* Initialize new supplemental page table */
void supplemental_page_table_init(struct supplemental_page_table *spt UNUSED) {
  hash_init(&spt->hash_pt, page_hash, page_less, NULL);
}

/* Copy supplemental page table from src to dst */
/* 부모 프로세스의 보조 페이지 테이블(src)을 자식 프로세스의 보조 페이지
 * 테이블(dst)로 복사합니다. */
bool supplemental_page_table_copy(struct supplemental_page_table *dst UNUSED,
                                  struct supplemental_page_table *src UNUSED) {
  struct hash_iterator i;

  // 부모 프로세스의 보조 페이지 테이블(src)을 반복 순회하는 반복자를
  // 초기화합니다.
  hash_first(&i, &src->hash_pt);
  while (hash_next(&i)) {
    // 보조 페이지 테이블의 엔트리를 가져옵니다.
    struct page *origin_p = hash_entry(hash_cur(&i), struct page, hash_elem);

    // 페이지의 유형, 가상 주소, 쓰기 가능 여부를 가져옵니다.
    enum vm_type type = page_get_type(origin_p); // vm_anon
    void *upage = origin_p->va;
    bool writable = origin_p->writable;

    if (origin_p->operations->type == VM_UNINIT) {
      // 페이지가 초기화되지 않은 상태인 경우, 초기화 함수와 보조 데이터를
      // 사용하여 페이지를 할당합니다.
      vm_initializer *init = origin_p->uninit.init;
      void *aux = origin_p->uninit.aux;

      if (!vm_alloc_page_with_initializer(type, upage, writable, init, aux))
        goto err;
    } else {
      // 페이지가 초기화된 상태인 경우, 페이지를 할당하고 데이터를 복사합니다.
      if (!vm_alloc_page(type, upage, writable))
        goto err;

      if (!vm_claim_page(upage))
        goto err;

      // 페이지 프레임의 내용을 원본에서 복사합니다.
      memcpy(upage, origin_p->frame->kva, PGSIZE);
    }
  }
  return true;
err:
  return false;
}

/* Free the resource hold by the supplemental page table */
/* 보조 페이지 테이블에서 할당된 자원을 해제하고 변경된 내용을 저장합니다. */
void supplemental_page_table_kill(struct supplemental_page_table *spt UNUSED) {
  hash_clear(&spt->hash_pt, clear_func);
}

/* Returns a hash value for page p. */
/* 이 함수는 주어진 페이지 p_의 해시 값을 계산합니다. 이 값은 주어진 페이지를
 * 해시 테이블의 버킷 중 하나에 매핑하는 데 사용됩니다. */
unsigned page_hash(const struct hash_elem *p_, void *aux UNUSED) {
  const struct page *p = hash_entry(p_, struct page, hash_elem);

  return hash_bytes(&p->va, sizeof(p->va));
}

/* Returns true if page a precedes page b. */
/* 두 페이지를 비교하여 첫 번째 페이지 a가 두 번째 페이지 b보다 앞에 있으면
 * true를 반환하고, 그렇지 않으면 false를 반환합니다. */
bool page_less(const struct hash_elem *a_, const struct hash_elem *b_,
               void *aux UNUSED) {
  const struct page *a = hash_entry(a_, struct page, hash_elem);
  const struct page *b = hash_entry(b_, struct page, hash_elem);

  return a->va < b->va;
}

/* 페이지와 관련된 리소스를 해제하는 해시 테이블을 위한 클리어 함수. */
void clear_func(struct hash_elem *elem, void *aux) {
  // 엔트리를 해시 테이블에서 가져와서 페이지 구조체로 형변환합니다.
  struct page *page = hash_entry(elem, struct page, hash_elem);

  // 페이지를 해제하는 함수를 호출, 페이지와 관련된 메모리 리소스를 해제
  vm_dealloc_page(page);
}