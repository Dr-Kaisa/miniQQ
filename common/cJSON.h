/*
  版权所有 (c) 2009-2017 Dave Gamble 和 cJSON 贡献者

  特此免费授予任何获得此软件及相关文档文件（“软件”）副本的人
  不受限制地处理软件的权限，包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售软件副本的权利，并允许获得该软件的人员这样做，但须符合以下条件：

  以上版权声明和本许可声明应包含在软件的所有副本或主要部分中。

  该软件按“原样”提供，不提供任何形式的明示或暗示保证，包括但不限于对适销性、特定用途适用性和非侵权的保证。在任何情况下，作者或版权持有人均不对因本软件或本软件的使用或其他交易而产生的任何索赔、损害或其他责任负责，无论是在合同诉讼、侵权诉讼或其他诉讼中。
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

  /* 在为 Windows 编译时，我们指定特定的调用约定，以避免在从具有不同默认调用约定的项目中调用时出现问题。对于 Windows，您有 3 个定义选项：

  CJSON_HIDE_SYMBOLS - 在您不想导出符号时定义此项
  CJSON_EXPORT_SYMBOLS - 在您想要导出符号时在库构建中定义此项（默认）
  CJSON_IMPORT_SYMBOLS - 如果您想要导入符号，请定义此项

  对于支持可见性属性的 *nix 构建，您可以通过以下方式定义类似行为：

  通过添加以下内容将默认可见性设置为隐藏：
  -fvisibility=hidden（用于 gcc）
  或
  -xldscope=hidden（用于 sun cc）
  添加到 CFLAGS

  然后使用 CJSON_API_VISIBILITY 标志以与 CJSON_EXPORT_SYMBOLS 相同的方式“导出”符号

  */

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* 默认导出符号，这是必要的，以便复制和粘贴 C 和头文件 */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type) type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type) __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type) __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* 项目版本 */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 15

#include <stddef.h>

/* cJSON 类型： */
#define cJSON_Invalid (0)
#define cJSON_False (1 << 0)
#define cJSON_True (1 << 1)
#define cJSON_NULL (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw (1 << 7) /* 原始 json */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

  /* cJSON 结构体： */
  typedef struct cJSON
  {
    /* next/prev 允许您遍历数组/对象链。或者，使用 GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* 数组或对象项将有一个子指针指向数组/对象中的项链。 */
    struct cJSON *child;

    /* 项的类型，如上所述。 */
    int type;

    /* 如果 type==cJSON_String 和 type == cJSON_Raw，则为项的字符串。 */
    char *valuestring;
    /* 不建议写入 valueint，请改用 cJSON_SetNumberValue */
    int valueint;
    /* 如果 type==cJSON_Number，则为项的数字。 */
    double valuedouble;

    /* 项的名称字符串，如果此项是对象的子项，或在对象的子项列表中。 */
    char *string;
  } cJSON;

  typedef struct cJSON_Hooks
  {
    /* malloc/free 在 Windows 上无论编译器的默认调用约定如何，都是 CDECL，因此确保钩子允许直接传递这些函数。 */
    void *(CJSON_CDECL *malloc_fn)(size_t sz);
    void(CJSON_CDECL *free_fn)(void *ptr);
  } cJSON_Hooks;

  typedef int cJSON_bool;

/* 限制嵌套数组/对象的深度，超出此限制后 cJSON 将拒绝解析它们。
 * 这是为了防止堆栈溢出。 */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

  /* 以字符串形式返回 cJSON 的版本 */
  CJSON_PUBLIC(const char *)
  cJSON_Version(void);

  /* 提供 malloc、realloc 和 free 函数给 cJSON */
  CJSON_PUBLIC(void)
  cJSON_InitHooks(cJSON_Hooks *hooks);

  /* 内存管理：调用者始终负责释放所有 cJSON_Parse（使用 cJSON_Delete）和 cJSON_Print（使用 stdlib free、cJSON_Hooks.free_fn 或 cJSON_free）的结果。唯一的例外是 cJSON_PrintPreallocated，在这种情况下，调用者完全负责缓冲区。*/
  /* 提供一块 JSON 数据，并返回一个可以查询的 cJSON 对象。 */
  CJSON_PUBLIC(cJSON *)
  cJSON_Parse(const char *value);
  CJSON_PUBLIC(cJSON *)
  cJSON_ParseWithLength(const char *value, size_t buffer_length);
  /* ParseWithOpts 允许您要求（并检查）JSON 是以空字符结束的，并检索指向最终解析字节的指针。*/
  /* 如果您在 return_parse_end 中提供一个指针且解析失败，那么 return_parse_end 将包含一个指向错误的指针，因此将匹配 cJSON_GetErrorPtr()。 */
  CJSON_PUBLIC(cJSON *)
  cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
  CJSON_PUBLIC(cJSON *)
  cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

  /* 将 cJSON 实体渲染为文本以便传输/存储。 */
  CJSON_PUBLIC(char *)
  cJSON_Print(const cJSON *item);
  /* 将 cJSON 实体渲染为无格式文本以便传输/存储。 */
  CJSON_PUBLIC(char *)
  cJSON_PrintUnformatted(const cJSON *item);
  /* 使用缓冲策略将 cJSON 实体渲染为文本。prebuffer 是对最终大小的估计。估计得越准确，重新分配的可能性越小。fmt=0 给出无格式，=1 给出格式化 */
  CJSON_PUBLIC(char *)
  cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
  /* 使用已分配内存的缓冲区将 cJSON 实体渲染为给定长度的文本。成功返回 1，失败返回 0。*/
  /* 注意：cJSON 并不总是 100% 准确地估计它将使用多少内存，所以为安全起见，请分配比实际需要多 5 个字节 */
  CJSON_PUBLIC(cJSON_bool)
  cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
  /* 删除一个 cJSON 实体及其所有子实体。 */
  CJSON_PUBLIC(void)
  cJSON_Delete(cJSON *item);

  /* 返回数组（或对象）中的项数。 */
  CJSON_PUBLIC(int)
  cJSON_GetArraySize(const cJSON *array);
  /* 从数组“array”中检索第“index”项。如果不成功，则返回 NULL。*/
  CJSON_PUBLIC(cJSON *)
  cJSON_GetArrayItem(const cJSON *array, int index);
  /* 从对象中获取“string”项。不区分大小写。 */
  CJSON_PUBLIC(cJSON *)
  cJSON_GetObjectItem(const cJSON *const object, const char *const string);
  CJSON_PUBLIC(cJSON *)
  cJSON_GetObjectItemCaseSensitive(const cJSON *const object, const char *const string);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_HasObjectItem(const cJSON *object, const char *string);
  /* 用于分析失败的解析。此函数返回一个指向解析错误的指针。您可能需要回看几个字符才能理解它。当 cJSON_Parse() 返回 0 时定义。cJSON_Parse() 成功时为 0。 */
  CJSON_PUBLIC(const char *)
  cJSON_GetErrorPtr(void);

  /* 检查项类型并返回其值 */
  CJSON_PUBLIC(char *)
  cJSON_GetStringValue(const cJSON *const item);
  CJSON_PUBLIC(double)
  cJSON_GetNumberValue(const cJSON *const item);

  /* 这些函数检查项的类型 */
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsInvalid(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsFalse(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsTrue(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsBool(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsNull(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsNumber(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsString(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsArray(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsObject(const cJSON *const item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_IsRaw(const cJSON *const item);

  /* 这些函数创建一个适当类型的 cJSON 项。 */
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateNull(void);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateTrue(void);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateFalse(void);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateBool(cJSON_bool boolean);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateNumber(double num);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateString(const char *string);
  /* 原始 json */
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateRaw(const char *raw);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateArray(void);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateObject(void);

  /* 创建一个字符串，其中 valuestring 引用一个字符串，因此
   * 不会被 cJSON_Delete 释放 */
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateStringReference(const char *string);
  /* 创建一个仅引用其元素的对象/数组，因此
   * 不会被 cJSON_Delete 释放 */
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateObjectReference(const cJSON *child);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateArrayReference(const cJSON *child);

  /* 这些实用程序创建一个具有 count 项的数组。
   * 参数 count 不能大于 number 数组中的元素数量，否则数组访问将超出边界。*/
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateIntArray(const int *numbers, int count);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateFloatArray(const float *numbers, int count);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateDoubleArray(const double *numbers, int count);
  CJSON_PUBLIC(cJSON *)
  cJSON_CreateStringArray(const char *const *strings, int count);

  /* 将项追加到指定的数组/对象中。*/
  CJSON_PUBLIC(cJSON_bool)
  cJSON_AddItemToArray(cJSON *array, cJSON *item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
  /* 当 string 明确定义为 const（即文字或类似情况）并且肯定会在 cJSON 对象中存活时，请使用此方法。
   * 警告：当使用此函数时，请确保始终检查 (item->type & cJSON_StringIsConst) 是否为零，然后再
   * 写入 `item->string` */
  CJSON_PUBLIC(cJSON_bool)
  cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
  /* 将引用项附加到指定的数组/对象中。当您想要将现有 cJSON 添加到新 cJSON 中，但不想破坏现有 cJSON 时，请使用此方法。*/
  CJSON_PUBLIC(cJSON_bool)
  cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

  /* 从数组/对象中移除/分离项。 */
  CJSON_PUBLIC(cJSON *)
  cJSON_DetachItemViaPointer(cJSON *parent, cJSON *const item);
  CJSON_PUBLIC(cJSON *)
  cJSON_DetachItemFromArray(cJSON *array, int which);
  CJSON_PUBLIC(void)
  cJSON_DeleteItemFromArray(cJSON *array, int which);
  CJSON_PUBLIC(cJSON *)
  cJSON_DetachItemFromObject(cJSON *object, const char *string);
  CJSON_PUBLIC(cJSON *)
  cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
  CJSON_PUBLIC(void)
  cJSON_DeleteItemFromObject(cJSON *object, const char *string);
  CJSON_PUBLIC(void)
  cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

  /* 更新数组项。*/
  CJSON_PUBLIC(cJSON_bool)
  cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* 将现有项向右移动。 */
  CJSON_PUBLIC(cJSON_bool)
  cJSON_ReplaceItemViaPointer(cJSON *const parent, cJSON *const item, cJSON *replacement);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_ReplaceItemInObject(cJSON *object, const char *string, cJSON *newitem);
  CJSON_PUBLIC(cJSON_bool)
  cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object, const char *string, cJSON *newitem);

  /* 复制一个 cJSON 项 */
  CJSON_PUBLIC(cJSON *)
  cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
  /* Duplicate 将在新内存中创建一个与传递的 cJSON 项相同的新项，
   * 需要释放。recurse!=0 时，它会复制连接到该项的任何子项。
   * item->next 和 ->prev 指针在从 Duplicate 返回时始终为零。*/
  /* 递归地比较两个 cJSON 项的相等性。如果 a 或 b 为空或无效，则它们将被视为不相等。
   * case_sensitive 决定对象键是区分大小写（1）还是不区分大小写（0） */
  CJSON_PUBLIC(cJSON_bool)
  cJSON_Compare(const cJSON *const a, const cJSON *const b, const cJSON_bool case_sensitive);

  /* 压缩字符串，移除字符串中的空白字符（如 ' '、'\t'、'\r'、'\n'）。
   * 输入指针 json 不能指向只读地址区域，如字符串常量，
   * 而应指向可读写的地址区域。*/
  CJSON_PUBLIC(void)
  cJSON_Minify(char *json);

  /* 用于在创建和添加项到对象的同时创建项。
   * 它们返回添加的项或失败时返回 NULL。*/
  CJSON_PUBLIC(cJSON *)
  cJSON_AddNullToObject(cJSON *const object, const char *const name);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddTrueToObject(cJSON *const object, const char *const name);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddFalseToObject(cJSON *const object, const char *const name);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddBoolToObject(cJSON *const object, const char *const name, const cJSON_bool boolean);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddNumberToObject(cJSON *const object, const char *const name, const double number);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddStringToObject(cJSON *const object, const char *const name, const char *const string);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddRawToObject(cJSON *const object, const char *const name, const char *const raw);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddObjectToObject(cJSON *const object, const char *const name);
  CJSON_PUBLIC(cJSON *)
  cJSON_AddArrayToObject(cJSON *const object, const char *const name);

/* 当分配整数值时，它需要同时传播到 valuedouble。*/
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
  /* cJSON_SetNumberValue 宏的辅助函数 */
  CJSON_PUBLIC(double)
  cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
  /* 更改 cJSON_String 对象的 valuestring，仅当对象类型为 cJSON_String 时生效 */
  CJSON_PUBLIC(char *)
  cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* 如果对象不是布尔类型，则此操作无效并返回 cJSON_Invalid，否则返回新类型 */
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False | cJSON_True))) ? (object)->type = ((object)->type & (~(cJSON_False | cJSON_True))) | ((boolValue) ? cJSON_True : cJSON_False) : cJSON_Invalid)

/* 用于迭代数组或对象的宏 */
#define cJSON_ArrayForEach(element, array) for (element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

  /* 使用通过 cJSON_InitHooks 设置的 malloc/free 函数来 malloc/free 对象 */
  CJSON_PUBLIC(void *)
  cJSON_malloc(size_t size);
  CJSON_PUBLIC(void)
  cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
