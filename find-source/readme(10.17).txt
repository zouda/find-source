update 10.17: 
config.txt 改为两行
删除apilist.txt

config.txt
line1: project name(mongo-master)
line2: all project path(/home/yoyo/work)


运行步骤：
1 删除result.csv文件
2 运行./browse /home/yoyo/work , 生成filelist.txt及config.txt
3 运行./buildconfig 生成compile_commands.json
4 运行./run 生成result.csv
 
