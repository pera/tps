#!/usr/bin/perl 

use strict;
use warnings;

# la forma "perl-ish" de hacerlo seria:
# if ($ARGV[0] eq reverse($ARGV[0])) {
#	print "es palindromo\n";
# }
#
# o hasta mas corto estilo oneliner:
# exit ($ARGV[0] eq reverse($ARGV[0])); 

if (@ARGV != 1) {
	print "Cantidad de argumentos incorrecta.\n";
	exit 1;
}

# perl es un lenguaje de weak typing
# en perl los strings son un tipo de dato basico (no se tratan como arreglos)
my @palabra = split(//,$ARGV[0]);

# Creo instancias de Cola y Pila:
my $cola = Cola->new();
my $pila = Pila->new();

foreach my $c (@palabra) {
	$cola->poner($c);
	$pila->poner($c);
}

my $espalindromo=1;

while (!($cola->vacia()) and $espalindromo) {
	my $c = $cola->sacar();
	my $p = $pila->sacar();
	if ($c ne $p) {
		$espalindromo = 0;
	}
}

if ($espalindromo and (!($cola->vacia()))) {
		$espalindromo = 0;
}

if ($espalindromo) {
	print "La palabra es un palindromo\n";
} else {
	print "La palabra NO es un palindromo\n";
}

### Superclase xIFO ###
{
	# Nombramos el namespace:
	package xIFO;

	# Una clase que sera usada como constructor:
	sub new {
		# el parametro de entrada (@_) es el nombre del namespace/clase
		my ($class) = @_;
		# $object contiene los _objetos_ como son definidos por Perl
		# y este es un hash que contiene un array etiquetado XIFO
		my $object = { XIFO => [] };
		# por ultimo "bendigo" el objeto indicandole que pertenece a este
		# namespace/clase (este segundo parametro no es necesario)
		my $self = bless $object, $class;

		# bless retorna un puntero (similar a "this") el cual contiene
		# nuestros objetos, y sera pasado de forma automatica como parametro
		# de entrada a todos los metodos del namespace actual.
		return $self;
	}

	# Perl no dispone en el core de funciones virtuales, pero con esto evito
	# que se utilice la clase xIFO directamente (en realidad evita el poner)
	sub poner {
		die("Este metodo es una interfaz...");
	}

	sub sacar {
		my ($self) = @_;
		return pop $self->{ XIFO };
	}

	sub vacia {
		my ($self) = @_;
		if (@{$self->{ XIFO }}) {
			return 0;
		} else {
			return 1;
		}
	}

	sub llena {
		return 0;
	}
}

### Clase Pila ###
{
	package Pila;

# Si las clases estuvieran en archivos separados (.pm) entonces usariamos:
#	our @ISA = qw(xIFO);
# pero por comodidad dejamos todo aca y usaremos base o parent (preferible)

	use parent -norequire, 'xIFO';

	sub poner {
		my ($self, $dato) = @_;
		push $self->{ XIFO }, $dato;
	}
}

### Clase Cola ###
{
	package Cola;

	use parent -norequire, 'xIFO';

	sub poner {
		my ($self, $dato) = @_;
		unshift $self->{ XIFO }, $dato;
	}
}

