#include <stdlib.h>
#include <mem.h>
#include <stdio.h>
#include <stdarg.h>
#include "LINEAR_SEQUENCE.h"
 
#define IS_INVALID(handle)(handle == LSQ_HandleInvalid)
 
typedef struct {
    LSQ_BaseTypeT *SeqData;
    int RealSize, MemSize;
} Sequence;
 
typedef struct {
    Sequence* seq;
    LSQ_IntegerIndexT index;
} Iter;
 
LSQ_HandleT LSQ_CreateSequence(void) {
    Sequence* s = (Sequence*) malloc(sizeof(Sequence));
    s->MemSize = 1;
    s->RealSize = 0;
    s->SeqData = (LSQ_BaseTypeT*)malloc(sizeof(LSQ_BaseTypeT) * s->MemSize);
    return s;
}
 
void LSQ_DestroySequence(LSQ_HandleT handle) {
    if (IS_INVALID(handle)){
        return;
    }
    Sequence* s = (Sequence*) handle;
    free(s->SeqData);
    free(handle);
}
 
LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
   return (IS_INVALID(handle) ? -1 : ((Sequence*) handle)->RealSize);
}
 
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    return ((!LSQ_IsIteratorPastRear(iterator)) && (!LSQ_IsIteratorBeforeFirst(iterator)) && (iterator != NULL));
}
 
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    return((iterator != NULL) && (((Iter*)iterator)->index >= ((Iter*)iterator)->seq->RealSize));
}
 
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    return(iterator != NULL && ((Iter*)iterator)->index < 0);
}
 
LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    if (!LSQ_IsIteratorDereferencable(iterator)){
        return LSQ_HandleInvalid;
    }
    LSQ_BaseTypeT* ans = (((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index);
    LSQ_BaseTypeT a = *ans;
    return ans;
}
 
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    if (handle == LSQ_HandleInvalid) return (NULL);
    Iter* iterator = (Iter*)malloc(sizeof(Iter));
    iterator->index = index;
    iterator->seq = (Sequence*)handle;
    return ((LSQ_IteratorT)iterator);
}
 
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    return IS_INVALID(handle) ? LSQ_HandleInvalid : LSQ_GetElementByIndex(handle, 0);
}
 
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    return IS_INVALID(handle) ? LSQ_HandleInvalid : LSQ_GetElementByIndex(handle, ((Sequence*)handle)->RealSize);
}
 
void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
   if (IS_INVALID(iterator)){
        return;
    }
    free(iterator);
}
 
void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if ((Iter*)iterator == NULL){
        return;
    }
    int c;
    c=((Iter*)iterator)->index;
    ((Iter*)iterator)->index += shift;
    c=((Iter*)iterator)->index;
    int f;
    f=c;
}
 
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    LSQ_ShiftPosition(iterator, 1);
}
 
void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    LSQ_ShiftPosition(iterator, -1);
}
 
void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if ((Iter*)iterator == NULL){
        return;
    }
    ((Iter*)iterator)->index = pos;
}
 
void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    if (IS_INVALID(iterator)){
        return;
    }
    if (((Iter*)iterator)->seq->RealSize == ((Iter*)iterator)->seq->MemSize){
        ((Iter*)iterator)->seq->MemSize *= 2;
        ((Iter*)iterator)->seq->SeqData = (LSQ_BaseTypeT *)realloc(((Iter*)iterator)->seq->SeqData,((Iter*)iterator)->seq->MemSize * sizeof(LSQ_BaseTypeT));
    }
    ((Iter*)iterator)->seq->RealSize++;
    memmove(((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index + 1,
            ((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index,
            sizeof(LSQ_BaseTypeT) * (((Iter*)iterator)->seq->RealSize - ((Iter*)iterator)->index - 1));
    *(((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index) = newElement;
}
 
void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if (handle == NULL){
        return;
    }
    Iter* iterator = (Iter*)LSQ_GetElementByIndex(handle, 0);
    if (iterator == NULL)
        return;
    LSQ_InsertElementBeforeGiven(iterator, element);
    LSQ_DestroyIterator(iterator);
}
 
void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if (handle == NULL){
        return;
    }
    Iter* iterator = (Iter*)(LSQ_GetElementByIndex(handle, ((Sequence*)handle)->RealSize));
    if (iterator == NULL)
        return;
    LSQ_InsertElementBeforeGiven(iterator, element);
    LSQ_DestroyIterator(iterator);
}
 
void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
    if (!LSQ_IsIteratorDereferencable(iterator))
    {
        return;
    }
    ((Iter*)iterator)->seq->RealSize--;
    memmove(((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index,
            ((Iter*)iterator)->seq->SeqData + ((Iter*)iterator)->index + 1,
            sizeof(LSQ_BaseTypeT) * (((Iter*)iterator)->seq->RealSize - ((Iter*)iterator)->index));
    if (((Iter*)iterator)->seq->RealSize <= ((Iter*)iterator)->seq->MemSize * 0.25) {
        int newSize = ((Iter*)iterator)->seq->MemSize / 2;
        if (((Iter*)iterator)->seq->MemSize == 0)
            newSize = 1;
        ((Iter*)iterator)->seq->MemSize = newSize;
        ((Iter*)iterator)->seq->SeqData = (LSQ_BaseTypeT *)realloc(((Iter*)iterator)->seq->SeqData, newSize * sizeof(LSQ_BaseTypeT));
    }
}
 
void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    if  (handle == NULL)
        return;
    Iter* iterator = (Iter*)(LSQ_GetElementByIndex(handle, 0));
    if (iterator == NULL){
        return;
    }
    LSQ_DeleteGivenElement(iterator);
    LSQ_DestroyIterator(iterator);
}
 
void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    if  (handle == NULL)
        return;
    Iter* iterator = (Iter*)(LSQ_GetElementByIndex(handle,((Sequence*)handle)->RealSize - 1));
    if (iterator == NULL)
        return;
    LSQ_DeleteGivenElement(iterator);
    LSQ_DestroyIterator(iterator);
}