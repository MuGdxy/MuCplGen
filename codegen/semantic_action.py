import os
import time

count = 16

src = open('Production.h','r')
dst = open('Production_gen.h','w')
content = src.read()

allcontent = '''//--{
//CodeGen by Python
//date: $$DATE$$
$$CODE$$
//--}
'''

f = open('set_action.h','w')


code = '''$$TEMPLATE$$
void SetAction(std::function<Ret($$FUNC_ARGS$$)> action)
{
    semantic_action = [action, this](std::vector<std::any*> data)->std::any*
    {
        $$GET_ARGS$$
        return RecordRet(action($$ARGS$$), data);
    };
}
'''
get_arg_j = 'auto&& arg$j$ = GetArg<Arg$j$>(data, $j$);\n'


allcode = ''

for i in range(0, count):
    template = 'template<class Ret'
    func_args = ''
    get_arg = ''
    arg = ''
    arg_count = i
    for j in range(0, i + 1):
        func_arg = "Arg" + str(j)
        template += ', class ' + func_arg
        get_arg += get_arg_j.replace('$j$', str(j))
        if(j != i):
            func_args += func_arg + ', '
            arg+='arg' + str(j) + ', '
        else: 
            func_args += func_arg
            arg += 'arg' + str(j)
    template += '>' 
    tmp_code = code
    tmp_code = tmp_code.replace('$$TEMPLATE$$',template)
    tmp_code = tmp_code.replace('$$FUNC_ARGS$$',func_args)
    tmp_code = tmp_code.replace('$$GET_ARGS$$',get_arg)
    tmp_code = tmp_code.replace('$$ARGS$$',arg)
    allcode += tmp_code
    #f.write(tmp_code)
    
allcontent = allcontent.replace("$$CODE$$", allcode)
allcontent = allcontent.replace("$$DATE$$", time.strftime("%Y-%m-%d", time.localtime()) )    

dst.write(allcontent)
f.close()