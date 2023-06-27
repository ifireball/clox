/* test_runner.c - Main test runner */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct dll_node {
  const char* str;
  struct dll_node* next;
  struct dll_node* prev;
};

int dll_node_count(const struct dll_node *dll) {
  int cnt = 0;
  const struct dll_node *dll_ptr = dll;
  while(dll_ptr) {
    ++cnt;
    dll_ptr = dll_ptr->next;
    if(dll_ptr == dll) break;
  }
  return cnt;
}

char* dll_to_string(const struct dll_node *dll) {
  char* str = calloc(1, sizeof(char));
  const struct dll_node *dll_ptr = dll;
  while(dll_ptr) {
    size_t old_len = strlen(str);
    size_t add_len = strlen(dll_ptr->str);
    char *newstr = calloc(old_len + add_len + 2, sizeof(char));
    strcpy(newstr, str);
    strcpy(&newstr[old_len], dll_ptr->str);
    strcpy(&newstr[old_len + add_len], "\n");
    str = newstr;
    dll_ptr = dll_ptr->next;
    if(dll_ptr == dll) break;
  }
  return str;
}

struct dll_node* new_dll(const char* first_str) {
  struct dll_node *new_node = calloc(1, sizeof(struct dll_node));
  new_node->str = calloc(strlen(first_str), sizeof(char));
  strcpy((char *)new_node->str, first_str);
  new_node->next = new_node;
  new_node->prev = new_node;
  return new_node;
}

void delete_dll(struct dll_node* dll) {
  struct dll_node *dll_ptr = dll;
  while(dll_ptr) {
    struct dll_node* next = dll_ptr->next;
    free((void*)dll_ptr->str);
    free(dll_ptr);
    dll_ptr = next;
    if(dll_ptr == dll) break;
  }  
}

struct dll_node* append_dll(struct dll_node* dll1, struct dll_node* dll2) {
  if(!dll1 || !dll2) return dll1 ? dll1 : dll2;
  struct dll_node* dll1_last = dll1->prev;
  struct dll_node* dll2_last = dll2->prev;
  dll1_last->next = dll2;
  dll2->prev= dll1_last;
  dll1->prev = dll2_last;
  dll2_last->next = dll1;
  return dll1;
}

struct dll_node* dll_find(struct dll_node* dll, const char* str) {
  struct dll_node* dll_ptr = dll;
  while(dll_ptr) {
    if(strcmp(dll_ptr->str, str) == 0) return dll_ptr;
    dll_ptr = dll_ptr->next;
    if(dll_ptr == dll) break;
  }
  return NULL;
}

bool test_assertion(const char* test_name, bool condition) {
  printf("%s: %s", test_name, condition ? "SUCCESS\n" : "FAILURE\n");
  return condition;
}

const struct dll_node* one_node_fixture() {
  static struct dll_node one_node_dll = {"foobar", NULL, NULL};
  one_node_dll.next = &one_node_dll;
  one_node_dll.prev = &one_node_dll;
  return &one_node_dll;
}

const struct dll_node* two_node_fixture() {
  static struct dll_node node_1 = {"foobar", NULL, NULL};
  static struct dll_node node_2 = {"bazbal", NULL, NULL};
  node_1.next = &node_2;
  node_1.prev = &node_2;
  node_2.next = &node_1;
  node_2.prev = &node_1;
  return &node_1;
}

bool test_dll_node_count_counts_to_0() {
  return test_assertion(__func__, dll_node_count(NULL) == 0);
}

bool test_dll_node_count_counts_to_1() {
  return test_assertion(__func__, dll_node_count(one_node_fixture()) == 1);
}

bool test_dll_node_count_counts_to_2() {
  return test_assertion(__func__, dll_node_count(two_node_fixture()) == 2);
}

bool dll_str_cmp(const struct dll_node* dll, const char* cmp_str) {
  char *str = dll_to_string(dll);
  bool rv = (strcmp(str, cmp_str) == 0);
  free(str);
  return rv;
}

bool test_dll_to_string_returns_empty() {
  return test_assertion(__func__, dll_str_cmp(NULL, ""));
}

bool test_dll_to_string_returns_for_1_elem() {
  return test_assertion(__func__, dll_str_cmp(one_node_fixture(), "foobar\n"));
}

bool test_dll_to_string_returns_for_2_elem() {
  return test_assertion(__func__, dll_str_cmp(two_node_fixture(), "foobar\nbazbal\n"));
}

bool test_new_dll_creates_one_node() {
  const char *inp_str = "foo";
  struct dll_node* dll = new_dll(inp_str);
  bool rv = test_assertion(
    __func__,
    dll
    && dll->str
    && dll->str != inp_str
    && strcmp(dll->str, inp_str) == 0
    && dll_node_count(dll) == 1
    && dll_str_cmp(dll, "foo\n")
  );
  delete_dll(dll);
  return rv;
}

bool test_append_dll_does_nothing_with_2_nulls() {
  return test_assertion(__func__, append_dll(NULL, NULL) == NULL);
}

bool test_append_dll_does_nothing_with_1st_null() {
  const char *inp_str = "foo";
  struct dll_node* dll = new_dll(inp_str);
  bool rv = test_assertion(
    __func__,
    append_dll(NULL, dll) == dll
    && dll_str_cmp(dll, "foo\n")
  );
  delete_dll(dll);
  return rv;
}

bool test_append_dll_does_nothing_with_2nd_null() {
  const char *inp_str = "foo";
  struct dll_node* dll = new_dll(inp_str);
  bool rv = test_assertion(
    __func__,
    append_dll(dll, NULL) == dll 
    && dll_str_cmp(dll, "foo\n")
  );
  delete_dll(dll);
  return rv;
}

bool test_append_dll_connects_2_nodes() {
  struct dll_node* dll1 = new_dll("foo");
  struct dll_node* dll2 = new_dll("bar");
  bool rv = test_assertion(
    __func__,
    append_dll(dll1, dll2) == dll1
    && dll_str_cmp(dll1, "foo\nbar\n")
    && dll_str_cmp(dll2, "bar\nfoo\n")
  );
  delete_dll(dll1);
  return rv;
}

bool test_append_dll_connects_4_nodes() {
  struct dll_node* dll1 = append_dll(new_dll("foo"), new_dll("bar"));
  struct dll_node* dll2 = append_dll(new_dll("baz"), new_dll("bal"));
  bool rv = test_assertion(
    __func__,
    append_dll(dll1, dll2) == dll1
    && dll_str_cmp(dll1, "foo\nbar\nbaz\nbal\n")
    && dll_str_cmp(dll2, "baz\nbal\nfoo\nbar\n")
  );
  delete_dll(dll1);
  return rv;
}

bool test_dll_find_works_w_null() {
  return test_assertion(__func__, dll_find(NULL, "foo") == NULL);
}

struct dll_node *four_heap_node_fixture() {
  return append_dll(
    new_dll("foo"),
    append_dll(
      new_dll("bar"),
      append_dll(
        new_dll("baz"),
        new_dll("bal")
      )
    )
  );
}

#define test_find_assertion(fn, find_str, eq_expr) \
  __extension__ ({ \
    struct dll_node *dll = four_heap_node_fixture(); \
    bool rv = test_assertion(fn, dll_find(dll, (find_str)) == (eq_expr)); \
    delete_dll(dll); \
    rv; \
  })


bool test_dll_find_locates_1st_node() {
  return test_find_assertion(__func__, "foo", dll);
}

bool test_dll_find_locates_2nd_node() {
  return test_find_assertion(__func__, "bar", dll->next);
}

bool test_dll_find_locates_4th_node() {
  return test_find_assertion(__func__, "bal", dll->next->next->next);
}

bool test_dll_find_returns_null_when_not_found() {
  return test_find_assertion(__func__, "bam", NULL);
}

int main() {
  bool success = (
    test_dll_node_count_counts_to_0()
    && test_dll_node_count_counts_to_1()
    && test_dll_node_count_counts_to_2()
    && test_dll_to_string_returns_empty()
    && test_dll_to_string_returns_for_1_elem()
    && test_dll_to_string_returns_for_2_elem()
    && test_new_dll_creates_one_node()
    && test_append_dll_does_nothing_with_2_nulls()
    && test_append_dll_does_nothing_with_1st_null()
    && test_append_dll_does_nothing_with_2nd_null()
    && test_append_dll_connects_2_nodes()
    && test_append_dll_connects_4_nodes()
    && test_dll_find_works_w_null()
    && test_dll_find_locates_1st_node()
    && test_dll_find_locates_2nd_node()
    && test_dll_find_locates_4th_node()
    && test_dll_find_returns_null_when_not_found()
  );

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
