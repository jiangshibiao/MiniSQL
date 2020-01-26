create table student (
		sno char(8),
		sname char(16) unique,
		sage int,
		sgender char (1),
		primary key ( sno )
);
select * from student;
insert into student values ('12345678','wy',22,'M');
insert into student values ('88888888','lgl',23,'F');
insert into student values ('89574954','jsb',24,'F');
insert into student values ('2888888','jsb',23,'F');
select * from student where sage > 20 and sgender = 'F';
delete from student where sno = '88888888';
drop table student;
