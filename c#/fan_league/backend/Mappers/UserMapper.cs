using fan_league.Entities;
using fan_league.Requests.Users;
using fan_league.Responses.Users;

namespace fan_league.Mappers;

public static class UserMapper
{
    public static User ToEntity(this CreateUserRequest request)
    {
        return new User
        {
            Username = request.Username,
            Email = request.Email,
            PasswordHash = string.Empty
        };
    }

    public static UserResponse ToResponse(this User user)
    {
        return new UserResponse(
            user.Id,
            user.Username,
            user.Email,
            user.CreatedAt,
            user.UpdatedAt);
    }

    public static void Apply(this UpdateUserRequest request, User user)
    {
        user.Username = request.Username;
        user.Email = request.Email;
        user.UpdatedAt = DateTime.UtcNow;
    }
}
