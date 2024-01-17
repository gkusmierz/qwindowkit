create schema carts;

set schema 'carts';

create sequence carts_seq
	as integer
	start with 100001;

alter sequence carts_seq owner to protone;

create table cart
(
	_root_id integer default nextval('carts_seq'::regclass) not null,
	_id integer default 1 not null,
	cart_id varchar not null
		constraint cart_idx
			unique,
	name varchar not null,
	constraint cart_pk
		primary key (_root_id, _id)
);

alter table cart owner to protone;