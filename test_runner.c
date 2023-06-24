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

bool test_dll_to_string_returns_empty() {
  char *str = dll_to_string(NULL);
  bool rv = test_assertion(__func__, strcmp(str, "") == 0);
  free(str);
  return rv;
}

bool test_dll_to_string_returns_for_1_elem() {
  char *str = dll_to_string(one_node_fixture());
  bool rv = test_assertion(__func__, strcmp(str, "foobar\n") == 0);
  free(str);
  return rv;
}

bool test_dll_to_string_returns_for_2_elem() {
  char *str = dll_to_string(two_node_fixture());
  bool rv = test_assertion(__func__, strcmp(str, "foobar\nbazbal\n") == 0);
  free(str);
  return rv;
}

bool test_add_dll_node_allocates_node() {

  return test_assertion(__func__, false);
}

int main() {
  bool success = true;
  success &= test_dll_node_count_counts_to_0();
  success &= test_dll_node_count_counts_to_1();
  success &= test_dll_node_count_counts_to_2();
  success &= test_dll_to_string_returns_empty();
  success &= test_dll_to_string_returns_for_1_elem();
  success &= test_dll_to_string_returns_for_2_elem();

  // success &= test_add_dll_node_allocates_node();
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
