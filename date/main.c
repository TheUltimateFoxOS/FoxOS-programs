#include <sys/time.h>
#include <buildin/unix_time.h>
#include <stdio.h>

int main() {
	long long int unix_time = _time();

	char date[27];
	unix_time_to_string(unix_time, date);

	printf("%s\n", date);

	return 0;
}