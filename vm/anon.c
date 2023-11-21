/* anon.c: Implementation of page for non-disk image (a.k.a. anonymous page). */

#include "vm/vm.h"
#include "devices/disk.h"

/* DO NOT MODIFY BELOW LINE */
static struct disk *swap_disk;
static bool anon_swap_in(struct page *page, void *kva);
static bool anon_swap_out(struct page *page);
static void anon_destroy(struct page *page);

struct bitmap *used_sector;
static int total_swap_page;

/* DO NOT MODIFY this struct */
static const struct page_operations anon_ops = {
	.swap_in = anon_swap_in,
	.swap_out = anon_swap_out,
	.destroy = anon_destroy,
	.type = VM_ANON,
};

/* Initialize the data for anonymous pages */
void vm_anon_init(void)
{
	/* TODO: Set up the swap_disk. */
	swap_disk = disk_get(1, 1);
	total_swap_page = disk_size(swap_disk) / 8;
	used_sector = bitmap_create(disk_size(swap_disk));
}

/* Initialize the file mapping */
bool anon_initializer(struct page *page, enum vm_type type, void *kva)
{
	/* Set up the handler */
	// printf("아논 이니셜\n");
	page->operations = &anon_ops;
	struct anon_page *anon_page = &page->anon;
}

/* Swap in the page by read contents from the swap disk. */
static bool
anon_swap_in(struct page *page, void *kva)
{
	// ASSERT(page == NULL);
	struct anon_page *anon_page = &page->anon;
	struct thread *curr = thread_current();
	for (int i = 0; i < 8; i++)
		disk_read(swap_disk, page->frame->start_sector + i, page->frame->kva + (512 * i));
	bitmap_set_multiple(used_sector, page->frame->start_sector, 8, false);
	page->frame->start_sector = 0;
	pml4_set_page(curr->pml4, page->va, kva, page->writable);
	// printf("스왑인\n");
	return true;
}


/* Swap out the page by writing contents to the swap disk. */
static bool
anon_swap_out(struct page *page)
{
	struct anon_page *anon_page = &page->anon;
	struct thread *curr = thread_current();
	size_t free_sector = bitmap_scan_and_flip(used_sector, 0, 8, false);
	// printf("스왑아웃\n");
	for (int i = 0; i < 8; i++)
		disk_write(swap_disk, free_sector + i, page->frame->kva + (512 * i));
	page->frame->start_sector = free_sector;
	pml4_clear_page(curr->pml4, page->va);
	page->frame->page = page;
	// page->frame->page = NULL;
	// page->frame = NULL;
	return true;
}

/* Destroy the anonymous page. PAGE will be freed by the caller. */
static void
anon_destroy(struct page *page)
{
	struct thread *curr = thread_current();
	struct anon_page *anon_page = &page->anon;
	if (page->frame != NULL)
	{
		pml4_clear_page(curr->pml4, page->va);
		if (page->frame->link_cnt == 0)
		{
			palloc_free_page(page->frame->kva);
			free(page->frame);
		}
		else
		{
			list_remove(&page->frame_page_list_elem);
			page->frame->link_cnt--;
		}
	}
}
