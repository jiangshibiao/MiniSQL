select * from t1 where name = 'Jim';
select * from t1 where name != 'Jim';
select * from t1 where name >= 'Queen';
select * from t1 where name <= 'Jim';

create index iname on t1(name);

select * from t1 where name = 'Jim';
select * from t1 where name != 'Jim';
select * from t1 where name >= 'Queen';
select * from t1 where name <= 'Jim';