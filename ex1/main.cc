#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>

using namespace llvm;
using namespace std;
void test_context()
{
/*
LLVMContext:
 type
 constant uniquing table
Should be one context per thread
Design pattern:
LLVMContext<->LLVMContextImpl

LLVMContextImpl:
modules --
maps --
*/
	cout<<"LLVMContext test"<<endl;
	LLVMContext* a = new LLVMContext;
	cout<<"dbg kind id:"<<a->getMDKindID("dbg")<<endl;
}
void test_builder()
{
/*
IRBuilder
create a instruction and insert into instruction list

BasicBlock

*/
    cout<<"IRBuilder test"<<endl;
	LLVMContext* a = new LLVMContext;
	(void)(a);
	IRBuilder<>* b = new IRBuilder<>(*a);
	(void)(b);
}
void test_module()
{
    cout<<"Module test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module*c = new Module("moduleA",*a);
	(void)(a);
	(void)(b);
	(void)(c);
}
void test_array()
{
    cout<<"ArrayType test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
	/* create function type
	void entry(void);
	*/
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
	/* create a basic block */
	BasicBlock* bb = BasicBlock::Create(*a,"entry",f);
        b->SetInsertPoint(bb);
	/* create a array type with size
	int a[10];
	*/
	ArrayType* at = ArrayType::get(Type::getInt32Ty(*a),10);
	/* create a array var 
	int a[10];
	*/
	AllocaInst* ai = b->CreateAlloca(at);
	Value* indices[]={
	b->getInt32(0),//for array itself
	b->getInt32(1)//for array's element
	};
	/* create gep to access array element */
	Value* a0 = b->CreateGEP(at,ai,indices);
	/* create a store to array element */
	b->CreateStore(b->getInt32(0),a0);
	/* crrate return void */
	b->CreateRetVoid();
	c->dump();
}
void test_struct()
{
    cout<<"StructType test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
    
	/* create function type
	i8* malloc(i32)
	*/
    Type* parms[]={
        Type::getInt32Ty(*a)
    };
    FunctionType* ft1 = FunctionType::get( PointerType::get(Type::getInt8Ty(*a),0),parms,false/*var args flag*/);
    Function* f1 = Function::Create(ft1,Function::ExternalLinkage,"malloc",c);
    /*
    void foo()
    */
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
    
    /* create a basic block */
	BasicBlock* bb = BasicBlock::Create(*a,"entry",f);
    b->SetInsertPoint(bb);
    
	/* create a array type with size
    Under LLVM, ArrayType must be sized.
    If Type without size then need more information in symbol table 
	type a=array of int //
    var a1=a [10] of [0]//
    
    -----------------------------------
    symbol| type    | llvm info | value
    a1    | array a | ArrayType | a1_val
    
    Access:
    a1[1]=1
    %a = GEP a1_val,0,1
    store i32 1,i32* %a
    
    
    type b={x:int,y:int}
    var b1=b{x=1,y=1}
    -----------------------------------
    symbol| type     | llvm info  | value
    b1    | struct b | StructType | b1_val
    
    Access:
    b1.x=1
    %b=GEP b1_val,0,0
    store i32 1,i32* %b
    
    basic type
    ------------------------------------
    symbol   | type       | llvm info
    "int"    | TypeInt    | Int32Ty
    "string" | TypeString | Int8*  <PointerType>
    
    
	*/
    ArrayType* at = ArrayType::get(Type::getInt32Ty(*a),10);
    Type* types[]={
    PointerType::get(at,0)
    };
	StructType* st = StructType::get(*a,types,false);
	/* create a struct var 
	int a[10];
	*/
	AllocaInst* ai = b->CreateAlloca(st);
    /* create array storage */
    Value* d = b->CreateCall(f1,b->getInt32(40));
    /* cast */
    d = b->CreateBitCast(d,PointerType::get(at,0));
    /*
    struct{
        [10*i32]*
    }
    */
	Value* indices[]={
	b->getInt32(0),//for struct itself
	b->getInt32(0),//for array pointer
	};
	/* create gep to access array element */
	Value* a0 = b->CreateGEP(st,ai,indices);
    //a0 = b->CreateLoad(PointerType::get(at,0),a0);
    b->CreateStore(d,a0);
    Value* indices1[]={
	b->getInt32(0),//for array pointer itself
	b->getInt32(1),//for array pointer
	};
    
    a0 = b->CreateGEP(at,d,indices1);
	/* create a store to array element */
	b->CreateStore(b->getInt32(0),a0);
	/* crrate return void */
	b->CreateRetVoid();
	c->dump();
}
void test_d_array()
{
    cout<<"dynamic array test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
    
    ArrayType* at = ArrayType::get(Type::getInt32Ty(*a),10);

    
	/* create function type
	i8* malloc(i32)
	*/
    Type* parms[]={
        Type::getInt32Ty(*a)
    };
    FunctionType* ft1 = FunctionType::get( PointerType::get(Type::getInt8Ty(*a),0),parms,false/*var args flag*/);
    Function* f1 = Function::Create(ft1,Function::ExternalLinkage,"malloc",c);
    /*
    void foo()
    */
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
    
    /* create a basic block */
	BasicBlock* bb = BasicBlock::Create(*a,"entry",f);
    b->SetInsertPoint(bb);
    
    /* create array storage */
    Value* d = b->CreateCall(f1,b->getInt32(40));
    
    d = b->CreateBitCast(d,PointerType::get(Type::getInt32Ty(*a),0));
    
    // calc element address
    Value* a0 = b->CreateGEP( Type::getInt32Ty(*a), d, b->getInt32(1) );
	/* create a store to array element */
	b->CreateStore(b->getInt32(1),a0);
	/* crrate return void */
	b->CreateRetVoid();
	c->dump();
}
void test_for()
{
    cout<<"For test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
    /*
    void foo()
    */
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
    
    /* create a basic block */
	BasicBlock* bb = BasicBlock::Create(*a,"entry",f);
    b->SetInsertPoint(bb);
    /*
    sum=0
    for i = 1 to 10
    sum+=i
    
    sum=0
    let
    i=1
    limit=10
    in
    while i<10
    sum+=i
    end
    
    loop:
    cond test
    true:
    body
    br loop
    false:
    
    */
    Value* sum,*i,*limit;
    sum = b->CreateAlloca(Type::getInt32Ty(*a));
    i = b->CreateAlloca(Type::getInt32Ty(*a));
    limit = b->CreateAlloca(Type::getInt32Ty(*a));
    b->CreateStore(b->getInt32(0),sum);
    b->CreateStore(b->getInt32(1),i);
    b->CreateStore(b->getInt32(10),limit);
    
    Value* limit_val = b->CreateLoad(limit);
    
    BasicBlock* loop = BasicBlock::Create(*a,"loop",f);
    BasicBlock* true_bb = BasicBlock::Create(*a,"true",f);
    BasicBlock* false_bb = BasicBlock::Create(*a,"false",f);
    
    b->CreateBr(loop);
    
    b->SetInsertPoint(loop);
    Value* i_val = b->CreateLoad(i);
    Value* sum_val = b->CreateLoad(sum);
    Value* cond = b->CreateICmp(CmpInst::ICMP_SLE,i_val,limit_val);
    b->CreateCondBr(cond,true_bb,false_bb);
    
    b->SetInsertPoint(true_bb);
    sum_val = b->CreateAdd(sum_val,i_val);
    b->CreateStore(sum_val,sum);
    i_val = b->CreateAdd(b->getInt32(1),i_val);
    b->CreateStore(i_val,i);
    b->CreateBr(loop);
    
    b->SetInsertPoint(false_bb);
    /* crrate return void */
	b->CreateRetVoid();
    c->dump();
}
void test_if()
{
    cout<<"If test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
    /*
    void foo()
    */
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
    
    /* create a basic block */
	BasicBlock* bb = BasicBlock::Create(*a,"entry",f);
    b->SetInsertPoint(bb);
    /*
    sum=0
    if sum>10
    sum=0 //if_statement
    else
    sum=1//else_statement
    
    */
    Value* sum;
    sum = b->CreateAlloca(Type::getInt32Ty(*a));

    b->CreateStore(b->getInt32(0),sum);

    
    Value* sum_val = b->CreateLoad(sum);
    
    BasicBlock* true_bb = BasicBlock::Create(*a,"true",f);
    BasicBlock* false_bb = BasicBlock::Create(*a,"false",f);
    BasicBlock* end_bb = BasicBlock::Create(*a,"end",f);

    Value* cond = b->CreateICmp(CmpInst::ICMP_SGT,sum_val,b->getInt32(10));
    b->CreateCondBr(cond,true_bb,false_bb);
    
    b->SetInsertPoint(true_bb);
    b->CreateStore(b->getInt32(0),sum);
    b->CreateBr(end_bb);
    
    b->SetInsertPoint(false_bb);
    b->CreateStore(b->getInt32(1),sum);
    b->CreateBr(end_bb);
    
    b->SetInsertPoint(end_bb);
    /* create return void */
    
	b->CreateRetVoid();
    c->dump();
}
void test_escape()
{
    /*
    function foo(){//escape list: 
    
        var a = 1
        function bar(){ //escape list: a
            a = 2 // get index in escape list, 
        }
        
        bar()
    }
    */
    cout<<"Escape test"<<endl;
	LLVMContext* a = new LLVMContext;
	IRBuilder<>* b = new IRBuilder<>(*a);
	Module* c = new Module("moduleA",*a);
    
    
    /*
    void foo()
    */
	FunctionType* ft = FunctionType::get(Type::getVoidTy(*a),false/*var args flag*/);
	Function* f = Function::Create(ft,Function::ExternalLinkage,"foo",c);
    BasicBlock* foo_bb = BasicBlock::Create(*a,"entry",f);
    b->SetInsertPoint(foo_bb);
    Value* a1 = b->CreateAlloca(Type::getInt32Ty(*a));
    b->CreateStore(b->getInt32(1),a1);
    
    /*
    void bar()
    */
    Type* parms[]={
        PointerType::get(Type::getInt32Ty(*a),0)
    };
	FunctionType* ft1 = FunctionType::get(Type::getVoidTy(*a),parms,false/*var args flag*/);
	Function* f1 = Function::Create(ft1,Function::ExternalLinkage,"bar",c);
    BasicBlock* bar_bb = BasicBlock::Create(*a,"entry",f1);
    b->CreateCall(f1,a1);
    b->CreateRetVoid();
    
    b->SetInsertPoint(bar_bb);
    for (auto &Arg : f1->args())
    {
        b->CreateStore(b->getInt32(2),&Arg);
    }
    b->CreateRetVoid();
    
    c->dump();
    
}
int main(int argc,char* argv[])
{
	//test_context();
	//test_struct();
    //test_for();
    test_d_array();
	return 0;
}
